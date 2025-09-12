#include "fat16.h"
#include "disk.h"
#include "log.h"
#include "stream.h"
#include <cstdint>
#include <stddef.h>

namespace filesystem {

#define FAT_ENTRY_SIZE 0x02

struct FAT_Header_Primary {
    uint8_t short_jmp_ins[3];
    uint8_t oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t fat_copies;
    uint16_t root_dir_entries;
    uint16_t number_of_sectors;
    uint8_t media_type;
    uint16_t sectors_per_fat;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_setors;
    uint32_t sectors_big;
} __attribute__((packed));

struct FAT_16_Header_Extended {
    uint8_t drive_number;
    uint8_t win_nt_bit;
    uint8_t signature;
    uint32_t volume_id;
    uint8_t volume_id_string[11];
    uint8_t system_id_string[8];
} __attribute__((packed));

struct FAT_Header {
    FAT_Header_Primary primary;
    union {
        FAT_16_Header_Extended fat16;
    } extended;
};

// Attributes of FAT_Directory_Item
#define FAT_FILE_READ_ONLY      0b00000001
#define FAT_FILE_HIDDEN         0b00000010
#define FAT_FILE_SYSTEM         0b00000100
#define FAT_FILE_VOLUME_LABEL   0b00001000
#define FAT_FILE_SUBDIRECTORY   0b00010000
#define FAT_FILE_ARCHIVED       0b00100000
#define FAT_FILE_DEVICE         0b01000000
#define FAT_FILE_RESERVED       0b10000000

struct FAT_Directory_Item {
    uint8_t filename[8];
    uint8_t ext[3];
    uint8_t attribute;
    uint8_t reserved;
    uint8_t creation_time_tenths_of_a_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access;
    uint16_t high_16_bits_first_cluster;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t low_16_bits_first_cluster;
    uint32_t filesize;
} __attribute__((packed));

struct FAT_Directory {
    ~FAT_Directory() {
        delete items;
    }

    FAT_Directory_Item *items;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct FAT_Item {
    ~FAT_Item() {
        if (type == FAT_DIRECTORY_ITEM)
            delete item;
        else if (type == FAT_DIRECTORY)
            delete directory;
    }

    union {
        FAT_Directory_Item *item;
        FAT_Directory *directory;
    };
    enum Type {
        FAT_DIRECTORY_ITEM,
        FAT_DIRECTORY,
    } type;
};

struct FAT_File_Descriptor : public FileDescriptor {
    ~FAT_File_Descriptor() {
        delete fat_item;
    }

    FAT_Item *fat_item;
    uint32_t pos;
};

struct FAT_Disk_Data {
    FAT_Header header;
    FAT_Directory root_directory;
    Unique<disk::Stream> cluster_read_stream;    // Used to stream data clusters
    Unique<disk::Stream> fat_read_stream;        // Used to stream the file allocation table
    Unique<disk::Stream> directory_stream;       // Used in situations where we stream the directory
};

int FAT16::CountItemsInDirectory(disk::Disk *disk, int start_sector_pos)
{
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    int directory_start_pos = start_sector_pos * disk->sectorSize();
    disk_data->directory_stream->Seek(directory_start_pos);

    int count = 0;
    FAT_Directory_Item file;
    while (true) {
        disk_data->directory_stream->Read((uint8_t *)&file, sizeof(FAT_Directory_Item));
        if (file.filename[0] == 0x00)
            break; // Finished
        if (file.filename[0] == 0xE5)
            continue; // Item is unused
        count++;
    }
    return count;
}

uint32_t FAT16::GetFirstCluster(const FAT_Directory_Item *item)
{
    return (item->high_16_bits_first_cluster << 16) | item->low_16_bits_first_cluster;
}

int FAT16::ClusterToSector(FAT_Disk_Data *disk_data, uint32_t cluster)
{
    return disk_data->root_directory.ending_sector_pos + ((cluster - 2) * disk_data->header.primary.sectors_per_cluster);
}

void FAT16::ReadInternal(disk::Disk *disk, uint32_t starting_cluster, int offset, int total, void *out)
{
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    disk::Stream *stream = disk_data->cluster_read_stream.data();
    ReadInternalFromStream(disk, stream, starting_cluster, offset, total, out);
}

void FAT16::ReadInternalFromStream(disk::Disk *disk, disk::Stream *stream, uint32_t cluster, int offset, int total, void *out)
{
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    int size_of_cluster_bytes = disk_data->header.primary.sectors_per_cluster * disk->sectorSize();
    int cluster_to_use = GetClusterForOffset(disk, cluster, offset);
    int offset_from_cluster = offset % size_of_cluster_bytes;
    int starting_sector = ClusterToSector(disk_data, cluster_to_use);
    int starting_pos = (starting_sector * disk->sectorSize()) + offset_from_cluster;
    stream->Seek(starting_pos);

    int total_to_read = total > size_of_cluster_bytes ? size_of_cluster_bytes : total;
    stream->Read((uint8_t *)out, total_to_read);

    total -= total_to_read;
    if (total > 0)
        ReadInternalFromStream(disk, stream, cluster, offset + total_to_read, total, (uint8_t *)out + total_to_read);
}

uint16_t FAT16::GetFATEntry(disk::Disk *disk, int cluster)
{
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    disk::Stream *stream = disk_data->fat_read_stream.data();
    uint32_t fat_table_position = disk_data->header.primary.reserved_sectors * disk->sectorSize();
    stream->Seek(fat_table_position + (cluster * FAT_ENTRY_SIZE));
    uint16_t result = 0;
    stream->Read((uint8_t *)&result, sizeof(result));
    return result;
}

int FAT16::GetClusterForOffset(disk::Disk *disk, uint32_t starting_cluster, int offset)
{
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    int size_of_cluster_bytes = disk_data->header.primary.sectors_per_cluster * disk->sectorSize();
    int cluster_to_use = starting_cluster;
    int clusters_ahead = offset / size_of_cluster_bytes;
    for (int i = 0; i < clusters_ahead; i++) {
        int entry = GetFATEntry(disk, cluster_to_use);
        // We are at the last entry in the file
        if (entry == 0xFFF8 || entry == 0xFFFF)
            return Status::E_IO;

        // Sector is marked as bad
        if (entry == 0xFF7)
            return Status::E_IO;

        // Reserved sector
        if (entry == 0xFF0 || entry == 0xFF6)
            return Status::E_IO;

        // Unused
        if (entry == 0x00)
            return Status::E_IO;

        cluster_to_use = entry;
    }

    return cluster_to_use;
}

FAT_Directory *FAT16::LoadDirectory(disk::Disk *disk, const FAT_Directory_Item *directory_item)
{
    if (!(directory_item->attribute & FAT_FILE_SUBDIRECTORY))
        return nullptr;

    FAT_Directory *directory = new FAT_Directory;

    int cluster = GetFirstCluster(directory_item);
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    int cluster_sector = ClusterToSector(disk_data, cluster);
    int total_items = CountItemsInDirectory(disk, cluster_sector);
    directory->total = total_items;

    int directory_size = total_items * sizeof(FAT_Directory_Item);
    // TODO: ownership
    directory->items = (FAT_Directory_Item *)calloc(directory_size);
    ReadInternal(disk, cluster, 0, directory_size, directory->items);

    // TODO: sector_pos, ending_sector_pos?

    return directory;
}

Unique<FAT_Item> FAT16::FindItemByNameInDirectory(disk::Disk *disk, const FAT_Directory &directory, const String &name)
{
    Unique<FAT_Item> ret(nullptr);
    for (int i = 0; i < directory.total; i++) {
        FAT_Directory_Item *item = &directory.items[i];

        // Normalize file name and extension
        char item_filename[13]; // filename[8] + '.' + ext[3] + '0'
        size_t pos = 0;
        unsigned char *c = item->filename;
        for (size_t i = 0; i < 12; i++, c++) {
            if (i == 8 && c[1] != 0x00 && c[1] != 0x20)
                item_filename[pos++] = '.';
            if (*c == 0x20 || *c == 0x00)
                continue;
            else
                item_filename[pos++] = tolower(*c);
        }
        item_filename[pos] = '\0';

        // TODO: Overload operator for this case
        if (strcmp(item_filename, name.c_str()) == 0) {
            ret.reset(new FAT_Item);
            if (item->attribute & FAT_FILE_SUBDIRECTORY) {
                ret->directory = LoadDirectory(disk, item);
                ret->type = FAT_Item::FAT_DIRECTORY;
            } else {
                ret->item = item;
                ret->type = FAT_Item::FAT_DIRECTORY_ITEM;
            }
            break;
        }
    }

    return ret;
}

bool FAT16::Resolve(disk::Disk *disk)
{
    FAT_Disk_Data *disk_data = new FAT_Disk_Data;
    disk->setFileSystemData(disk_data);

    // Streams
    disk_data->cluster_read_stream = Unique<disk::Stream>(new disk::Stream(disk->driver()));
    disk_data->fat_read_stream = Unique<disk::Stream>(new disk::Stream(disk->driver()));
    disk_data->directory_stream = Unique<disk::Stream>(new disk::Stream(disk->driver()));

    // Read the FAT headers
    disk::Stream stream(disk->driver());
    stream.Read((uint8_t *)&disk_data->header, sizeof(FAT_Header));
    if (disk_data->header.extended.fat16.signature != 0x29) {
        disk->setFileSystemData(nullptr);
        return false;
    }

    // Get the root directory
    FAT_Header_Primary *primary_header = &disk_data->header.primary;
    int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) + primary_header->reserved_sectors;
    int root_dir_entries = primary_header->root_dir_entries;
    int root_dir_size = root_dir_entries * sizeof(FAT_Directory_Item);
    int total_sectors = root_dir_size / disk->sectorSize();
    if (root_dir_size % disk->sectorSize())
        total_sectors += 1;

    FAT_Directory_Item *items = (FAT_Directory_Item *)calloc(root_dir_size);
    disk_data->directory_stream->Seek(disk->sectorsToBytes(root_dir_sector_pos));
    disk_data->directory_stream->Read((uint8_t *)items, root_dir_size);

    disk_data->root_directory.items = items;
    disk_data->root_directory.total = CountItemsInDirectory(disk, root_dir_sector_pos);
    disk_data->root_directory.sector_pos = root_dir_sector_pos;
    disk_data->root_directory.ending_sector_pos = root_dir_sector_pos + total_sectors;

    return true;
}

FileDescriptor *FAT16::Open(disk::Disk *disk, const Path &path)
{
    const Vector<String> &path_parts = path.components();
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();
    Unique<FAT_Item> root_item = FindItemByNameInDirectory(disk, disk_data->root_directory, path_parts[1]);
    if (!root_item) {
        log::error("FAT16::Open: Could not find root directory\n");
        return nullptr;
    }

    Unique<FAT_Item> current_item = move(root_item);
    for (size_t i = 2; i < path_parts.size(); i++) {
        const String &part = path_parts[i];
        current_item = FindItemByNameInDirectory(disk, *current_item->directory, part);
        if (current_item->type != FAT_Item::FAT_DIRECTORY) {
            log::error("FAT16::Open: Could not find directory '%s'\n", part.c_str());
            return nullptr;
        }
    }

    FAT_File_Descriptor *descriptor = new FAT_File_Descriptor;
    descriptor->fat_item = current_item.release();
    descriptor->pos = 0;
    return descriptor;
}

size_t FAT16::Read(FileDescriptor *descriptor, size_t size, size_t count, char *buffer)
{
    FAT_File_Descriptor *desc = (FAT_File_Descriptor *)descriptor;
    FAT_Directory_Item *item = desc->fat_item->item;
    int offset = desc->pos;
    for (uint32_t i = 0; i < count; i++) {
        ReadInternal(desc->disk, GetFirstCluster(item), offset, size, buffer);
        buffer += size;
        offset += size;
    }
    return count;
}

int FAT16::Seek(FileDescriptor *descriptor, size_t offset, FileSeekMode seek_mode)
{
    FAT_File_Descriptor *desc = (FAT_File_Descriptor *)descriptor;
    FAT_Item *fat_item = desc->fat_item;
    if (fat_item->type == FAT_Item::FAT_DIRECTORY) {
        log::error("Seek: Cannot seek in directory\n");
        return Status::E_INVALID_ARGUMENT;
    }

    if (fat_item->item->filesize <= offset) {
        log::error("Seek: Cannot seek past end of file\n");
        return Status::E_INVALID_ARGUMENT;
    }

    switch (seek_mode)
    {
    case FileSeekMode::SEEK_SET:
        desc->pos = offset;
        break;
    case FileSeekMode::SEEK_END:
        log::error("Seek: SEEK_END mode is unimplemented.\n");
        return Status::E_NOT_IMPLEMENTED;
    case FileSeekMode::SEEK_CUR:
        desc->pos += offset;
        break;
    default:
        log::error("Seek: Invalid seek mode\n");
        return Status::E_INVALID_ARGUMENT;
    }

    return 0;
}

int FAT16::Stat(FileDescriptor *descriptor, FileStat *stat)
{
    FAT_File_Descriptor *desc = (FAT_File_Descriptor *)descriptor;
    FAT_Item *fat_item = desc->fat_item;
    if (fat_item->type == FAT_Item::FAT_DIRECTORY) {
        log::error("Stat: Not yet implemented for directories\n");
        return Status::E_NOT_IMPLEMENTED;
    }

    FAT_Directory_Item *item = fat_item->item;
    stat->size = item->filesize;
    stat->flags = 0;
    if (item->attribute & FAT_FILE_READ_ONLY)
        stat->flags |= FileStatFlag::ReadOnly;

    return 0;
}

bool FAT16::Close(FileDescriptor *)
{
    // fclose will delete the descriptor
    return true;
}

}; // namespace filesystem

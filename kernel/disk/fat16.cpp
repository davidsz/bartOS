#include "fat16.h"
#include "disk.h"
#include "log.h"
#include "stream.h"
#include <cstdint>

namespace filesystem {

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
    FAT_Directory_Item *item;
    int total;
    int sector_pos;
    int ending_sector_pos;
};

struct FAT_Item {
    union {
        FAT_Item *file;
        FAT_Directory *directory;
    };
    enum Type {
        FAT_FILE,
        FAT_DIRECTORY,
    } type;
};

struct FAT_File_Descriptor : public FileDescriptor {
    FAT_Item *item;
    uint32_t pos;
};

struct FAT_Disk_Data {
    FAT_Header header;
    FAT_Directory root_directory;
    disk::Stream *cluster_read_stream;    // Used to stream data clusters
    disk::Stream *fat_read_stream;        // Used to stream the file allocation table
    disk::Stream *directory_stream;       // Used in situations where we stream the directory
};

int FAT16::CountItemsInDirectory(disk::Disk *disk, int start_sector_pos)
{
    FAT_Disk_Data *disk_data = (FAT_Disk_Data *)disk->fileSystemData();

    disk::Stream *stream = disk_data->directory_stream;
    int directory_start_pos = start_sector_pos * disk->sectorSize();
    stream->Seek(directory_start_pos);

    int count = 0;
    FAT_Directory_Item item;
    while (true) {
        stream->Read((uint8_t *)&item, sizeof(FAT_Directory_Item));
        if (item.filename[0] == 0x00)
            break; // Finished
        if (item.filename[0] == 0xE5)
            continue; // Item is unused
        count++;
    }

    return count;
}

bool FAT16::Resolve(disk::Disk *disk)
{
    FAT_Disk_Data *disk_data = new FAT_Disk_Data;
    disk->setFileSystemData(disk_data);

    // Streams
    disk_data->cluster_read_stream = new disk::Stream(disk->driver());
    disk_data->fat_read_stream = new disk::Stream(disk->driver());
    disk_data->directory_stream = new disk::Stream(disk->driver());

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
    int root_dir_size = root_dir_entries * sizeof(FAT_Directory);
    int total_sectors = root_dir_size / disk->sectorSize();
    if (root_dir_size % disk->sectorSize())
        total_sectors += 1;

    FAT_Directory_Item *item = (FAT_Directory_Item *)calloc(root_dir_size);
    disk_data->directory_stream->Seek(disk->sectorsToBytes(root_dir_sector_pos));
    disk_data->directory_stream->Read((uint8_t *)item, root_dir_size);

    disk_data->root_directory.item = item;
    disk_data->root_directory.total = CountItemsInDirectory(disk, root_dir_sector_pos);
    disk_data->root_directory.sector_pos = root_dir_sector_pos;
    disk_data->root_directory.ending_sector_pos = root_dir_sector_pos + total_sectors;

    return true;
}

FileDescriptor *FAT16::Open(disk::Disk *, const Path &)
{
    // TODO: Implement
    return nullptr;
}

}; // namespace filesystem

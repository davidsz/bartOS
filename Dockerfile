FROM ubuntu:latest
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get -y install \
    build-essential \
    grub2 mkisofs \
    git curl bison flex libgmp-dev libmpc-dev libmpfr-dev texinfo libisl-dev gcc-multilib g++-multilib

WORKDIR /home/ubuntu

ARG prefix=/home/ubuntu/opt/cross
ARG target=i686-elf

RUN export PATH="$prefix/bin:$PATH" && \
    echo "export PREFIX=$prefix"           >> /root/.bashrc && \
    echo "export TARGET=$target"           >> /root/.bashrc && \
    echo "export PATH=$prefix/bin:${PATH}" >> /root/.bashrc

# Build and install binutils
RUN git clone --depth 1 --branch binutils-2_44 git://sourceware.org/git/binutils-gdb.git binutils && \
    mkdir -p build-binutils && \
    cd build-binutils && \
    ../binutils/configure --target=$target --prefix=$prefix --with-sysroot --disable-nls --disable-werror && \
    make -j4 && \
    make install

# Build and install gcc
RUN git clone --depth 1 --branch releases/gcc-13.2.0 git://gcc.gnu.org/git/gcc.git gcc && \
    cd gcc && \
    ./contrib/download_prerequisites && \
    cd .. && \
    mkdir -p build-gcc && \
    cd build-gcc && \
    ../gcc/configure --target=$target --prefix=$prefix --disable-nls --enable-languages=c,c++ --without-headers --disable-hosted-libstdcxx && \
    make all-gcc -j4 && \
    make all-target-libgcc -j4 && \
    make all-target-libstdc++-v3 -j4 && \
    make install-gcc && \
    make install-target-libgcc && \
    make install-target-libstdc++-v3

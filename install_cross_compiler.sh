#!/bin/bash

# https://wiki.osdev.org/GCC_Cross-Compiler

set -xe

export PREFIX="$HOME/opt/cross" # installation directory
export TARGET=i686-elf # Only x86 32 bit for now
export PATH="$PREFIX/bin:$PATH"

BINUTILS_VERSION=2.37 # latest as of Jan 2022
GCC_VERSION=11.2.0 # latest as of Jan 2022

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &>/dev/null && pwd -P)

make_command="make -j $(nproc)"

#############################################
# DOWNLOAD
#############################################
wget https://ftp.gnu.org/gnu/binutils/binutils-$BINUTILS_VERSION.tar.gz
wget https://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz

#############################################
# EXTRACT
#############################################
mkdir -p $SCRIPT_DIR/src
tar -zxvf binutils-$BINUTILS_VERSION.tar.gz -C $SCRIPT_DIR/src
tar -zxvf gcc-$GCC_VERSION.tar.gz -C $SCRIPT_DIR/src

install() {
    for arch in "i686-elf"
    do
        TARGET=$arch
        #############################################
        # INSTALL BINUTILS
        #############################################
        cd $SCRIPT_DIR/src
        mkdir -p build-binutils
        mkdir -p build-binutils/$TARGET
        cd build-binutils/$TARGET
        ../../binutils-$BINUTILS_VERSION/configure \
            --target=$TARGET \
            --prefix=$PREFIX \
            --with-sysroot \
            --disable-nls \
            --disable-werror
        $make_command
        $make_command install

        #############################################
        # INSTALL GCC
        #############################################
        cd $SCRIPT_DIR/src
        mkdir -p build-gcc
        mkdir -p build-gcc/$TARGET
        cd build-gcc/$TARGET
        ../../gcc-$GCC_VERSION/configure \
            --target=$TARGET \
            --prefix="$PREFIX" \
            --disable-nls \
            --enable-languages=c,c++ \
            --without-headers
        $make_command all-gcc
        $make_command all-target-libgcc
        $make_command install-gcc
        $make_command install-target-libgcc
    done
}

install

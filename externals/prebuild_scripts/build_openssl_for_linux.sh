#!/bin/sh

# ####################################################
# Build script for assimp for Linux
# ####################################################


VERSION="3.2.1"
#if [ -n "$1" ]
#then
#    VERSION="$1"
#fi

ARCH=linux
ZIPFILE=${ARCH}_openssl_${VERSION}

clear
echo "Building openssl Version: $VERSION"

if [ ! -d "openssl-${VERSION}" ]; then
    wget https://www.openssl.org/source/openssl-${VERSION}.tar.gz
    tar -zxf openssl-${VERSION}.tar.gz
fi

cd "openssl-${VERSION}"

#export CC=clang
export PREFIX=$(pwd)/../$ZIPFILE

if [ ! -d "$PREFIX" ]
then
    mkdir $PREFIX
fi

#./config  --prefix=$PREFIX --openssldir=$PREFIX

./Configure ${x86_64}  --prefix=$PREFIX --openssldir=$PREFIX

if [ $? -ne 0 ]
then
    exit
fi

make
make install

cd ..

if [ -d "../prebuilt/${ZIPFILE}" ]
then
    rm -rf ../prebuilt/${ZIPFILE}
fi

mv ${ZIPFILE} ../prebuilt/

rm -rf openssl




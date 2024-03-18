#!/bin/sh

# ####################################################
# Build script for assimp for Linux
# ####################################################
# ATTENTION: if you system is not linux, you have to change
# the variable TOOLCHAIN (search below). E.g. on macos it is
# $ANDROID_NDK_HOME/toolchains/llvm/prebuilt/darwin-x86_64/bin

ANDROID_NDK_VERSION="r20b"

openssl_VERSION="3.2.1"
if [ -n "$1" ]
then
    openssl_VERSION="$1"
fi
WORK_PATH=$(cd "$(dirname "$0")";pwd)
if [ ! -d android-ndk-r20b ]
then
    wget https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
    unzip android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
fi
export ANDROID_NDK_HOME="android-ndk-r20b"
export ANDROID_NDK_PATH=${WORK_PATH}/${ANDROID_NDK_HOME}
export ANDROID_NDK_ROOT=${ANDROID_NDK_PATH}

ARCH=andV8
ZIPFILE=${ARCH}_openssl

clear
echo "Building openssl Version: $openssl_VERSION"

if [ ! -d "openssl-${openssl_VERSION}" ]; then
    wget https://www.openssl.org/source/openssl-${openssl_VERSION}.tar.gz
    tar -zxf openssl-${openssl_VERSION}.tar.gz
fi

cd openssl-${openssl_VERSION}

export PATH=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin:$ANDROID_NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin:$ANDROID_NDK_ROOT/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin:$PATH
export API=21
architecture=android-arm64

export PREFIX=$(pwd)/../$ZIPFILE

if [ ! -d "$PREFIX" ]
then
    mkdir $PREFIX
fi

./Configure ${architecture} -D__ANDROID_API__=$API --prefix=$PREFIX --openssldir=$PREFIX

if [ $? -ne 0 ]
then
    exit
fi

make
make install

cd ..

if [ -d "../prebuilt/${ARCH}_openssl-${openssl_VERSION}" ]
then
    rm -rf ../prebuilt/${ARCH}_openssl-${openssl_VERSION}
fi

mv ${ZIPFILE} ../prebuilt/${ARCH}_openssl-${openssl_VERSION}/


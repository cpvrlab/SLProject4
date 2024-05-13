#!/bin/sh

# ####################################################
# Build script for assimp for Linux
# ####################################################

assimp_VERSION="$1"
ARCH=andV8
ZIPFILE="$ARCH"_assimp_"$assimp_VERSION"
ZIPFOLDER=$ZIPFILE
BUILD_D="$ARCH"_debug_"$assimp_VERSION"
BUILD_R="$ARCH"_release_"$assimp_VERSION"
NDK_VERSION="26.2.11394342"

clear
echo "Building assimp Version: $assimp_VERSION"

if [ "$#" -lt 1 ]; then
    echo "No assimp tag passed as 1st parameter"
    exit
fi

# Cloning assimp
if [ ! -d "assimp/.git" ]; then
    git clone https://github.com/assimp/assimp
fi

# Get all assimp tags and check if the requested exists
cd assimp
git tag > assimp_tags.txt

if grep -Fx "$assimp_VERSION" assimp_tags.txt > /dev/null; then
    git checkout $assimp_VERSION
    git pull origin $assimp_VERSION
else
    echo "No valid assimp tag passed as 1st parameter !!!!!"
    exit
fi

# Make build folder for debug version
rm -rf $BUILD_D
mkdir $BUILD_D
cd $BUILD_D

# Run cmake to configure and generate the make files

cmake \
    -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk/$NDK_VERSION/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-N \
    -DCMAKE_INSTALL_PREFIX=install \
    -DCMAKE_BUILD_TYPE=Debug \
    -DASSIMP_BUILD_TESTS=OFF \
    -DBUILD_SHARED_LIBS=ON \
    ..

# finally build it
make -j100

# copy all into install folder
make install
cd .. # back to assimp

# Make build folder for release version
rm -rf $BUILD_R
mkdir $BUILD_R
cd $BUILD_R

# Run cmake to configure and generate the make files
cmake \
    -DCMAKE_TOOLCHAIN_FILE=~/Android/Sdk/ndk/$NDK_VERSION/build/cmake/android.toolchain.cmake \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-N \
    -DCMAKE_INSTALL_PREFIX=install \
    -DCMAKE_BUILD_TYPE=Release \
    -DASSIMP_BUILD_TESTS=OFF \
    -DBUILD_SHARED_LIBS=ON \
    ..

# finally build it
make -j100

# copy all into install folder
make install
cd .. # back to assimp

# Create zip folder for debug and release version
rm -rf $ZIPFOLDER
mkdir -p $ZIPFOLDER/include
mkdir -p $ZIPFOLDER/Release/arm64-v8a
mkdir -p $ZIPFOLDER/Debug/arm64-v8a

cp -R $BUILD_R/install/include   $ZIPFOLDER/
cp -R $BUILD_R/install/lib/*.so       $ZIPFOLDER/Release/arm64-v8a
cp -R $BUILD_D/install/lib/*.so       $ZIPFOLDER/Debug/arm64-v8a
cp LICENSE $ZIPFOLDER
cp Readme.md $ZIPFOLDER

if [ -d "../../prebuilt/$ZIPFILE" ]; then
    rm -rf ../../prebuilt/$ZIPFILE
fi

mv $ZIPFOLDER ../../prebuilt/

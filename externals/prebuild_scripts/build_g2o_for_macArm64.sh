#!/bin/sh
 
# ####################################################
# Build script for g2o for macArm64
# ####################################################
 
ARCH="macArm64"
VERSION="20170730_git"
ZIPFOLDER="$ARCH"_g2o_20170730
BUILD_D=build/"$ARCH"_debug
BUILD_R=build/"$ARCH"_release
clear
echo "Building g2o using the sources in the thirdparty directory"
if [ ! -d ../g2o ]
then
    echo "g2o directory doesn't exists"
    git clone https://github.com/RainerKuemmerle/g2o.git ../g2o
fi
 
echo "Building g2o using the sources in the thirdparty directory"
cd ../g2o
 
git checkout $VERSION
 
# Make build folder for debug version
mkdir build
rm -rf $BUILD_D
mkdir $BUILD_D
cd $BUILD_D
 
# Run cmake to configure and generate the make files
cmake \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_INSTALL_PREFIX=install \
    -DG2O_BUILD_APPS=off \
    -DG2O_BUILD_EXAMPLES=off \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_DEBUG_POSTFIX="" \
    -DEIGEN3_INCLUDE_DIR=../eigen \
    -DG2O_USE_OPENGL=off \
    ../..
 
# finally build it
make -j8
 
# copy all into install folder
make install
cd ../.. # back to g2o
 
# Make build folder for release version
rm -rf $BUILD_R
mkdir $BUILD_R
cd $BUILD_R
 
# Run cmake to configure and generate the make files
cmake \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_INSTALL_PREFIX=install \
    -DG2O_BUILD_APPS=off \
    -DG2O_BUILD_EXAMPLES=off \
    -DCMAKE_BUILD_TYPE=Release \
    -DEIGEN3_INCLUDE_DIR=../eigen \
    -DG2O_USE_OPENGL=off \
    ../..
 
# finally build it
make -j8
 
# copy all into install folder
make install
cd ../.. # back to g2o
 
# Create zip folder for debug and release version
rm -rf ../prebuilt/$ZIPFOLDER
mkdir ../prebuilt/$ZIPFOLDER
echo "dst is ../prebuilt/$ZIPFOLDER/"
cp -R $BUILD_R/install/include   ../prebuilt/$ZIPFOLDER/include
cp -R $BUILD_R/install/lib       ../prebuilt/$ZIPFOLDER/Release
cp -R $BUILD_D/install/lib       ../prebuilt/$ZIPFOLDER/Debug
cp doc/license* ../prebuilt/$ZIPFOLDER
cp README.md ../prebuilt/$ZIPFOLDER
 
#if [ -d "../prebuilt/$ZIPFILE" ]; then
#    rm -rf ../prebuilt/$ZIPFILE
#fi
 
#mv $ZIPFOLDER ../prebuilt
 

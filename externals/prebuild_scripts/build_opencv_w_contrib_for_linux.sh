#!/bin/sh

# ####################################################
# Build script for OpenCV with contributions for Linux
# ####################################################

CV_VERSION="$1"
ARCH=linux
ZIPFILE="$ARCH"_opencv_"$CV_VERSION"
ZIPFOLDER="build/$ZIPFILE"
BUILD_D="build/$ARCH"_debug_"$CV_VERSION"
BUILD_R="build/$ARCH"_release_"$CV_VERSION"

clear
echo "Building OpenCV Version: $CV_VERSION"

if [ "$#" -lt 1 ]; then
    echo "No OpenCV tag passed as 1st parameter"
    exit
fi

# Cloning OpenCV
if [ ! -d "opencv/.git" ]; then
    git clone https://github.com/opencv/opencv.git
fi

# Cloning OpenCV contributions
if [ ! -d "opencv_contrib/.git" ]; then
    git clone https://github.com/opencv/opencv_contrib.git
fi

# Get all OpenCV tags and check if the requested exists
cd opencv
git tag > opencv_tags.txt

if grep -Fx "$CV_VERSION" opencv_tags.txt > /dev/null; then
    git checkout $CV_VERSION
    git pull origin $CV_VERSION
    cd ..
    cd opencv_contrib
    git checkout $CV_VERSION
    git pull origin $CV_VERSION
    cd ..
else
    echo "No valid OpenCV tag passed as 1st parameter !!!!!"
    exit
fi

# Make build folder for debug version
cd opencv
mkdir build
rm -rf $BUILD_D
mkdir $BUILD_D
cd $BUILD_D

# Run cmake to configure and generate the make files
cmake \
-DCMAKE_CONFIGURATION_TYPES=Debug \
-DBUILD_WITH_DEBUG_INFO=ON \
-DCMAKE_INSTALL_PREFIX=./install \
-DBUILD_opencv_python_bindings_generator=OFF \
-DBUILD_opencv_python2=OFF \
-DBUILD_opencv_java_bindings_generator=OFF \
-DBUILD_opencv_cudacodec=OFF \
-DBUILD_PERF_TESTS=OFF \
-DBUILD_TESTS=OFF \
-DWITH_MATLAB=OFF \
-DBUILD_ZLIB=ON \
-DBUILD_PNG=ON \
-DBUILD_JPEG=ON \
-DBUILD_TIFF=ON \
-DBUILD_WEBP=ON \
-DBUILD_OPENEXR=ON \
-DOPENCV_ENABLE_NONFREE=ON \
-DOPENCV_EXTRA_MODULES_PATH=../../../opencv_contrib/modules \
../..

# finally build it
make -j16

# copy all into install folder
make install
cd ../.. # back to opencv

# Make build folder for release version
rm -rf $BUILD_R
mkdir $BUILD_R
echo "$BUILD_R and $BUILD_D"
cd $BUILD_R
#
# Run cmake to configure and generate the make files
cmake \
-DCMAKE_CONFIGURATION_TYPES=Release \
-DBUILD_WITH_DEBUG_INFO=OFF \
-DCMAKE_INSTALL_PREFIX=./install \
-DBUILD_opencv_python_bindings_generator=OFF \
-DBUILD_opencv_python2=OFF \
-DBUILD_opencv_java_bindings_generator=OFF \
-DBUILD_opencv_cudacodec=OFF \
-DBUILD_PERF_TESTS=OFF \
-DBUILD_TESTS=OFF \
-DWITH_MATLAB=OFF \
-DBUILD_ZLIB=ON \
-DBUILD_PNG=ON \
-DBUILD_JPEG=ON \
-DBUILD_TIFF=ON \
-DBUILD_WEBP=ON \
-DBUILD_OPENEXR=ON \
-DOPENCV_ENABLE_NONFREE=ON \
-DOPENCV_EXTRA_MODULES_PATH=../../../opencv_contrib/modules \
../..

# finally build it
make -j16

# copy all into install folder
make install
cd ../.. # back to opencv

# Create zip folder for debug and release version
rm -rf $ZIPFOLDER
mkdir -p "$ZIPFOLDER"

echo "$PWD/$BUILD_R/install/include $PWD/$ZIPFOLDER/include"
cp -R $BUILD_R/install/include   $ZIPFOLDER/include
cp -R $BUILD_R/install/lib       $ZIPFOLDER/Release
cp -R $BUILD_D/install/lib       $ZIPFOLDER/Debug

if [ -d  $BUILD_D/install/lib64 ]; then
    cp -R $BUILD_R/install/lib64 $ZIPFOLDER/Release
    cp -R $BUILD_D/install/lib64 $ZIPFOLDER/Debug
fi

cp LICENSE $ZIPFOLDER
cp README.md $ZIPFOLDER

if [ -d "../prebuilt/$ZIPFILE" ]; then
    rm -rf "../prebuilt/$ZIPFILE"
fi

if [ ! -d "../../prebuilt/" ]; then
    mkdir "../../prebuilt/"
fi


echo "ls . : $(ls .)"
echo "ls ../../ : $(ls ../../)"
echo "ls ../../prebuilt : $(ls ../../prebuilt)"


echo "cp -r $ZIPFOLDER ../../prebuilt"
echo "PWD IS $PWD"
echo "../../prebuilt IS $PWD/../../prebuilt"
cp -r "$ZIPFOLDER" "../../prebuilt"


echo "prebuilt content is $(ls ../../prebuilt)"

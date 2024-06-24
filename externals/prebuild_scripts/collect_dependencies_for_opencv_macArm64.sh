#!/bin/sh

# ##########################################################
# Collect OpenCV Dependencies for MacArm64
# ##########################################################

CV_VERSION="4.7.0"
ARCH="macArm64"
CONFIG="release"
DEST_DIR="3rdParty"
ZIPFOLDER=build/"$ARCH"_opencv_"$CV_VERSION"
OPENCV_LIB_DIR="$ZIPFOLDER"/"$CONFIG"

echo "================================================================="
echo "Collect OpenCV Dependencies for: $CV_VERSION for architecture: $ARCH"
echo "================================================================="

cd opencv

if [ ! -d "build" ]; then
    echo "No build inside OpenCV folder"
    exit
fi

cd $ZIPFOLDER

if [ ! -d $CONFIG ]; then
    echo "No $CONFIG folder"
    exit
fi

cd $CONFIG

mkdir -p $DEST_DIR

indent () {
    string="$1"
    num_spaces="$2"
    printf "%${num_spaces}s%s\n" '' "$string"
}

# Function to copy dependencies
copy_dependencies() {
    lib=$1
    depth=$2
    deps=$(otool -L $lib | grep /opt/homebrew/opt | awk '{print $1}')
    for dep in $deps; do
        #cp -R $dep $DEST_DIR
        dep=${dep%:} # remove trailing :
        if [ $1 != $dep ]; then
            echo $dep
            copy_dependencies $dep $((depth+1))
        fi
    done
}

#copy_dependencies libopencv_videoio.4.7.0.dylib
#copy_dependencies /opt/homebrew/opt/ffmpeg/lib/libavcodec.60.dylib 0

# Copy OpenCV libraries and their dependencies
for lib in libopencv_*.$CV_VERSION.dylib; do
 #   cp -R $lib $DEST_DIR
   echo "-------------------------------------------"
   echo $lib
   copy_dependencies $lib 0
done





#!/bin/bash

# Script abort on error
set -e

# Expected Settings via environment variables:
# COMPILER= gcc/clang
# LANGUAGE= C++98 / C++11
# QTVERSION= qt5
# BUILDTYPE= debug / eelease

echo "====================================="
echo "====================================="
echo "Sourcing CI/ci-linux-config.sh"
echo "====================================="
echo "====================================="
#include ci options script
MY_DIR=$(dirname $(readlink -f $0))
source CI/ci-linux-config.sh

echo "Building with path: $BUILDPATH"
echo "Full cmake options: $OPTIONS  "

echo "====================================="
echo "====================================="
echo "Cloning Test Data:"
echo "====================================="
echo "====================================="

rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@www.graphics.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData


echo "====================================="
echo "====================================="
echo "Building: in $BUILDPATH"
echo "====================================="
echo "====================================="

# Make release build folder
if [ ! -d $BUILDPATH ]; then
  mkdir $BUILDPATH
fi
cd $BUILDPATH

cmake -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE -DSTL_VECTOR_CHECKS=ON $OPTIONS ../

#build it
make $MAKE_OPTIONS

make $MAKE_OPTIONS test

echo "====================================="
echo "====================================="
echo "Collecting required libraries:"
echo "====================================="
echo "====================================="

# copy the used shared libraries to the lib folder
cd Build

# Creating System Library folder to contain all dependend libraries to run OpenFlipper
if [ ! -d systemlib ]; then
  echo "Creating systemlib folder"
  mkdir systemlib
fi

echo "Copying all required libraries of OpenFlipper to the systemlib directory"
ldd bin/OpenFlipper | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' systemlib
cd ../..


#echo "====================================="
#echo "====================================="
#echo "Collecting artifacts"
#echo "====================================="
#echo "====================================="

#create an artifact directory
#if [ ! -d artifacts ]; then
#  echo "Creating artifacts folder"
#  mkdir artifacts
#fi

#echo "Current directory is : $(pwd)"

# Copy all Build 
#rsync -aq * artifacts --exclude=artifacts --exclude=.git

# create an archive with all the build files so we can use them in the test script
#tar -cvf buildfiles.tar artifacts
#cd ..

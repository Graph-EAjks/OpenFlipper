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
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@gitlab.vci.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData


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

cmake -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE -DACG_BUILD_UNIT_TESTS=TRUE -DOPENMESH_BUILD_UNITTESTS=TRUE  -DSTL_VECTOR_CHECKS=ON $OPTIONS ../

if [ "$IWYU" == "yes" ]; then
  # do iwyu check
  if echo $(iwyu --version) | grep -q "0.11"
  then
    # support older tool version
    iwyu_tool -j 4 -p . -- \
    --mapping_file=/usr/share/include-what-you-use/qt5_4.imp \
    --mapping_file=/usr/share/include-what-you-use/gcc.libc.imp \
    --mapping_file=/usr/share/include-what-you-use/clang-6.intrinsics.imp \
    | tee iwyu.dump
  else
    # current tool version
    iwyu_tool -j 4 -p . -- \
    -Xiwyu --mapping_file=/usr/share/include-what-you-use/qt5_4.imp \
    -Xiwyu --mapping_file=/usr/share/include-what-you-use/gcc.libc.imp \
    -Xiwyu --mapping_file=/usr/share/include-what-you-use/clang-6.intrinsics.imp \
    | tee iwyu.dump
  fi

  cd ..
else
  #build it
  make $MAKE_OPTIONS

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

#   #use ldd to resolve the libs and use `patchelf --print-needed to filter out
#   # "magic" libs kernel-interfacing libs such as linux-vdso.so, ld-linux-x86-65.so or libpthread
#   # which you probably should not relativize anyway
#   join \
#      <(ldd "bin/OpenFlipper" |awk '{if(substr($3,0,1)=="/") print $1,$3}' |sort) \
#      <(patchelf --print-needed "bin/OpenFlipper" |sort) |cut -d\  -f2 |
#
#   #copy the lib selection to ./lib
#   xargs -d '\n' -I{} cp --copy-contents {} ./lib 
#   #make the relative lib paths override the system lib path
#   patchelf --set-rpath "\$ORIGIN/lib" "$1"
  echo "Copying all required libraries of OpenFlipper to the systemlib directory"
#  join <(ldd "bin/OpenFlipper" |awk '{if(substr($3,0,1)=="/") print $1,$3}' |sort) <(patchelf --print-needed "bin/OpenFlipper" |sort) |cut -d\  -f2 | xargs -d '\n' -I{} cp --copy-contents {} ./systemlib 
  join <(ldd lib/*.so bin/OpenFlipper ../tests/testBinaries/compareTool |awk '{if(substr($3,0,1)=="/") print $1,$3}' |sort -u ) <(patchelf --print-needed lib/*.so bin/OpenFlipper ../tests/testBinaries/compareTool |sort -u ) |cut -d\  -f2 | sort -u | xargs -d '\n' -I{} cp --copy-contents {} ./systemlib

  # Debug output of libraries copied:
  join <(ldd lib/*.so bin/OpenFlipper ../tests/testBinaries/compareTool |awk '{if(substr($3,0,1)=="/") print $1,$3}' |sort -u ) <(patchelf --print-needed lib/*.so bin/OpenFlipper ../tests/testBinaries/compareTool |sort -u ) |cut -d\  -f2 | sort -u 

  #ldd bin/OpenFlipper | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' systemlib
  cd ../..
fi

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

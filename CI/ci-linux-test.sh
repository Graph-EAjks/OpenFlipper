#!/bin/bash

# Script abort on error
set -e

# Expected Settings via environment variables:
# COMPILER= gcc/clang
# LANGUAGE= C++98 / C++11
# QTVERSION= qt5
# BUILDTYPE= debug / release

#include ci options script
MY_DIR=$(dirname $(readlink -f $0))
source CI/ci-linux-config.sh

echo "====================================="
echo "====================================="
echo "Current Directory structure:"
echo "====================================="
echo "====================================="

# copy artifact files to toplevel and remove subdirectory
#mv artifacts-$BUILDPATH artifacts
#rsync -a $MY_DIR/.. $MY_DIR/../..
#rm -rf artifacts

echo "====================================="
echo "====================================="
echo "Cloning Test Data:"
echo "====================================="
echo "====================================="

rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@www.graphics.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData

#########################################
# Run Release Unittests
#########################################
# Make release build folder
if [ ! -d $BUILDPATH ]; then
  echo "ERROR!!! Build Path not found !!!!"
  exit 10
else
  echo "Changing to build path $BUILDPATH"
  cd $BUILDPATH
fi


ls


# copy the used shared libraries to the lib folder
#cd Build

#if [ ! -d systemlib ]; then
#  mkdir systemlib
#fi

#ldd bin/OpenFlipper | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' systemlib
#cd ..

#clean old cmake cache as the path might have changed
find . -name "CMakeCache.txt" -type f -delete

#just to be safe clean the test file definitions too
if [ -f CTestTestfile.cmake ]
then
	echo "Removing old CTestTestfile.cmake"
	rm CTestTestfile.cmake
fi
#just to be safe clean the test file definitions too
if [ -f DartConfiguration.tcl ]
then
	echo "Removing old DartConfiguration.tcl"
	rm DartConfiguration.tcl
fi

# Run cmake to make sure the tests are configured correctly for this system
cmake -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE -DSTL_VECTOR_CHECKS=ON -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="iwyu" $OPTIONS ../

#tell the location to the libs from build jobs
export LD_LIBRARY_PATH=$(pwd)/Build/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/Build/systemlib:$LD_LIBRARY_PATH

# The tests should be build by the build job already!
#make test

echo "====================================="
echo "====================================="
echo "Test directory:"
echo "====================================="
echo "====================================="


cd tests

ls

echo "====================================="
echo "====================================="
echo "Running tests:"
echo "====================================="
echo "====================================="


python3 run_tests.py

cd ..


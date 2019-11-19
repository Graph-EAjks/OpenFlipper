#!/bin/bash

# Script abort on error
set -e

# Expected Settings via environment variables:
# COMPILER= gcc/clang
# LANGUAGE= C++98 / C++11
# QTVERSION= qt5
# BUILDTYPE= debug / eelease


#include ci options script
#MY_DIR=$(dirname $(readlink -f $0))
source CI/ci-linux-config.sh

echo "Building with path: $BUILDPATH"
echo "Full cmake options: $OPTIONS  "

########################################
# Fetch test data
########################################
rm -rf TestData
#git clone git@roosevelt:moebius/OpenFlipper-Test-Data.git TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@www.graphics.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData


#########################################
# Build Release version and Unittests
#########################################

# Make release build folder
if [ ! -d $BUILDPATH ]; then
  mkdir $BUILDPATH
fi

cd $BUILDPATH

cmake -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE -DSTL_VECTOR_CHECKS=ON $OPTIONS ../

#build it
make $MAKE_OPTIONS

cd ..

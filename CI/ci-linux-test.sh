#!/bin/bash

# Script abort on error
set -e

# Expected Settings via environment variables:
# COMPILER= gcc/clang
# LANGUAGE= C++98 / C++11
# QTVERSION= qt5
# BUILDTYPE= debug / release

#include ci options script
#MY_DIR=$(dirname $(readlink -f $0))
source CI/ci-linux-config.sh

########################################
# Fetch test data
########################################
rm -rf TestData
#git clone git@roosevelt:moebius/OpenFlipper-Test-Data.git TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@www.graphics.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData

#########################################
# Run Release Unittests
#########################################

cd $BUILDPATH

# copy the used shared libraries to the lib folder
cd Build

if [ ! -d systemlib ]; then
  mkdir systemlib
fi

ldd bin/OpenFlipper | grep "=> /" | awk '{print $3}' | xargs -I '{}' cp -v '{}' systemlib

cd ..

#tell the location to the libs from build jobs
export LD_LIBRARY_PATH=$(pwd)/Build/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=$(pwd)/Build/systemlib:$LD_LIBRARY_PATH

cd tests
bash run_tests.sh

cd ..

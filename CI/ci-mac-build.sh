#!/bin/bash

# Script abort on error
set -e
########################################
# Fetch test data
########################################
rm -rf TestData
git clone git@roosevelt:moebius/OpenFlipper-Test-Data.git TestData

########################################
# Build
########################################

if [ ! -d build-release ]; then
  mkdir build-release
fi

cd build-release

# Build without ports to avoid qt4 collision
/opt/local/bin/cmake ../ -DCMAKE_CXX_FLAGS='-std=c++11' -DQWT6_INCLUDE_DIR=/Users/jenkins/sw/qwt-6.1.3-qt5.11.3/lib/qwt.framework/Headers -DQWT6_LIBRARY=/Users/jenkins/sw/qwt-6.1.3-qt5.11.3/lib/qwt.framework -DQT5_INSTALL_PATH=/Users/jenkins/sw/Qt/5.11.3/clang_64/   

make -j2

# Required for the tests to build, if a package was generated
make fixbundle

cd ..

#create an artifact directory
if [ ! -d artifacts ]; then
  mkdir artifacts
fi

#cp -R * artifacts
rsync -a --exclude=artifacts --exclude=.git . ./artifacts

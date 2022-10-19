#!/bin/bash

# Script abort on error
set -e
########################################
# Fetch test data
########################################
rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@gitlab.vci.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData

########################################
# Build
########################################

if [ ! -d build-release ]; then
  mkdir build-release
fi

cd build-release



OPTIONS="-DQWT6_INCLUDE_DIR=/Users/jenkins/sw/qwt-6.1.3-qt5.11.3/lib/qwt.framework/Headers -DQWT6_LIBRARY=/Users/jenkins/sw/qwt-6.1.3-qt5.11.3/lib/qwt.framework -DQT_INSTALL_PATH=/Users/jenkins/sw/Qt/5.11.3/clang_64/ -DQT_VERSION=5"

echo $LANGUAGE

if [ "$LANGUAGE" == "cpp11" ]; then
  echo "Using C++11 standard";
  OPTIONS="$OPTIONS -DOPENFLIPPER_CXX_STANDARD=11 "
  BUILDPATH="$BUILDPATH-cpp11"
elif [ "$LANGUAGE" == "cpp17" ]; then
  echo "Using C++17 standard";
  OPTIONS="$OPTIONS -DOPENFLIPPER_CXX_STANDARD=17"
  BUILDPATH="$BUILDPATH-cpp17"
else
  echo "Unknown CXX standard!!!"
  BUILDPATH="$BUILDPATH-unknown"
fi

# Build without ports to avoid qt4 collision
/opt/local/bin/cmake ../ $OPTIONS

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

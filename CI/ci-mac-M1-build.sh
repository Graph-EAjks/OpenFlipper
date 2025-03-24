#!/bin/bash


PATH=/opt/homebrew/bin:/opt/homebrew/sbin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin
#PATH=/opt/homebrew/bin:/opt/local/bin:$PATH
export PATH


export LD_LIBRARY_PATH=/opt/homebrew/lib:/opt/local/lib

# Script abort on error
set -e


echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Basic configuration details:"
echo "======================================================================"
echo -e "${NC}"

echo "This is an Mac M1 build"

cmake --version

echo "Current PATH variable:"
echo "$PATH"
echo ""

echo "Current LD_LIBRARY_PATH variable:"
echo "$LD_LIBRARY_PATH"
echo ""


echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Building Release version"
echo "======================================================================"
echo -e "${NC}"

echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Fetching test data"
echo "======================================================================"
echo -e "${NC}"

rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@gitlab.vci.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData


echo -e "${OUTPUT}"
echo ""
echo "======================================================================"
echo "Configuring"
echo "======================================================================"
echo -e "${NC}"



if [ ! -d build-release ]; then
  mkdir build-release
fi

cd build-release

# Build without ports to avoid qt4 collision
cmake ../ -DQT_VERSION=6 

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

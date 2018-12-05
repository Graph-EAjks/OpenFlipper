#!/bin/bash

OPTIONS=""
MAKE_OPTIONS=""
BUILDPATH="build"

#set default Build type to Release
if [ "$BUILDTYPE" == "" ]; then
  BUILDTYPE="Release"
fi

# set buildpath according to buildtype
BUILDPATH="$BUILDPATH-$BUILDTYPE"

#set CMake build Type
OPTIONS="-DCMAKE_BUILD_TYPE=$BUILDTYPE"

if [ "$COMPILER" == "gcc" ]; then
  echo "Setting Compiler to GCC";
  BUILDPATH="$BUILDPATH-gcc"

  # without icecc: no options required
  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=/usr/lib/icecc/bin/g++ -DCMAKE_C_COMPILER=/usr/lib/icecc/bin/gcc"
  MAKE_OPTIONS="-j16"
  export ICECC_CXX=/usr/bin/g++ ; export ICECC_CC=/usr/bin/gcc

elif [ "$COMPILER" == "clang" ]; then

  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DGTEST_PREFIX=~/sw/gtest-1.8.0-clang/ "

#  Build options with icecc /not working due to strange symbol errors
#  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=/usr/lib/icecc/bin/g++ -DCMAKE_C_COMPILER=/usr/lib/icecc/bin/gcc -DGTEST_PREFIX=~/sw/gtest-1.7.0-clang/ "
#  export ICECC_CXX=/usr/bin/clang++ ; export ICECC_CC=/usr/bin/clang

  BUILDPATH="$BUILDPATH-clang"
  MAKE_OPTIONS="-j6"

  echo "Setting compiler to CLANG";
fi  

if [ "$LANGUAGE" == "C++98" ]; then
  echo "Using C++98 standard";
  BUILDPATH="$BUILDPATH-cpp98"
elif [ "$LANGUAGE" == "C++11" ]; then
  echo "Using C++11 standard";
  OPTIONS="$OPTIONS -DCMAKE_CXX_FLAGS='-std=c++11' "
  BUILDPATH="$BUILDPATH-cpp11"
fi  

if [ "$QTVERSION" == "QT5.11.2" ]; then
  echo "Using QT5.11.2";
  BUILDPATH="$BUILDPATH-qt5.11.2"
  OPTIONS="$OPTIONS -DQWT6_INCLUDE_DIR=~/sw/qwt-6.1.3-qt5.11.2/include -DQWT6_LIBRARY_DIR=~/sw/qwt-6.1.3-qt5.11.2/lib -DQWT6_LIBRARY=~/sw/qwt-6.1.3-qt5.11.2/lib/libqwt.so -DQT5_INSTALL_PATH=~/sw/Qt/5.11.2/gcc_64"
elif [ "$QTVERSION" == "QT5.9.0" ]; then
  echo "Using QT5.9.0";
  BUILDPATH="$BUILDPATH-qt5.9.0"
  OPTIONS="$OPTIONS -DFORCE_QT4=FALSE -DQWT6_INCLUDE_DIR=~/sw/qwt-6.1.3-qt5.9.0/include -DQWT6_LIBRARY_DIR=~/sw/qwt-6.1.3-qt5.9.0/lib -DQWT6_LIBRARY=~/sw/qwt-6.1.3-qt5.9.0/lib/libqwt.so -DQT5_INSTALL_PATH=~/sw/Qt/5.9/gcc_64"
fi

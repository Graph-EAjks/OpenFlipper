#!/bin/bash

OPTIONS=""
MAKE_OPTIONS=""
BUILDPATH="build"


#set default Build type to Release
if [ "$BUILDTYPE" == "" ]; then
  BUILDTYPE="release"
fi

# set buildpath according to buildtype
BUILDPATH="$BUILDPATH-$BUILDTYPE"

#set CMake build Type
OPTIONS="-DCMAKE_BUILD_TYPE=$BUILDTYPE"

if [ "$COMPILER" == "gcc" ]; then
  echo "Setting Compiler to GCC";
  BUILDPATH="$BUILDPATH-gcc"

  # without icecc: no options required
  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=/usr/lib/icecc/bin/g++ -DCMAKE_C_COMPILER=/usr/lib/icecc/bin/gcc -DGTEST_ROOT=$HOME/sw/gtest-1.10.0 "
  MAKE_OPTIONS="-j16"
  export ICECC_CXX=/usr/bin/g++ ; export ICECC_CC=/usr/bin/gcc

elif [ "$COMPILER" == "clang" ]; then

  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang -DGTEST_PREFIX=$HOME/sw/gtest-1.10.0-clang/ "

#  Build options with icecc /not working due to strange symbol errors
#  OPTIONS="$OPTIONS -DCMAKE_CXX_COMPILER=/usr/lib/icecc/bin/g++ -DCMAKE_C_COMPILER=/usr/lib/icecc/bin/gcc -DGTEST_PREFIX=$HOME/sw/gtest-1.7.0-clang/ "
#  export ICECC_CXX=/usr/bin/clang++ ; export ICECC_CC=/usr/bin/clang

  BUILDPATH="$BUILDPATH-clang"
  MAKE_OPTIONS="-j6"

  echo "Setting compiler to CLANG";
fi  

if [ "$LANGUAGE" == "cpp17" ]; then
  echo "Using C++17 standard";
  OPTIONS="$OPTIONS -DOPENFLIPPER_CXX_STANDARD=17"
  BUILDPATH="$BUILDPATH-cpp17"
else
  echo "Unknown CXX standard!!!"
  BUILDPATH="$BUILDPATH-unknown"
fi  

#TODO: Do we still need QWT paths here? If so, what is the path for QWT in QT 6.5.3? 
if [ "$QTVERSION" == "qt6.5.3" ]; then
  BUILDPATH="$BUILDPATH-qt6.5.3"
  OPTIONS="$OPTIONS -DQT_INSTALL_PATH=$HOME/sw/Qt/6.5.3/gcc_64"
  QT_INSTALL_PATH="$HOME/sw/Qt/6.5.3/gcc_64"
elif [ "$QTVERSION" == "qt6.7.0" ]; then
  BUILDPATH="$BUILDPATH-qt6.7.0"
  OPTIONS="$OPTIONS -DQWT6_INCLUDE_DIR=$HOME/sw/qwt-6.3.0-qt6.7.0/include -DQWT6_LIBRARY_DIR=$HOME/sw/qwt-6.3.0-qt6.7.0/lib -DQWT6_LIBRARY=$HOME/sw/qwt-6.3.0-qt6.7.0/lib/libqwt.so -DQT_INSTALL_PATH=$HOME/sw/Qt/6.7.0/gcc_64"
  QT_INSTALL_PATH="$HOME/sw/Qt/6.7.0/gcc_64"
fi

if [ "$PYTHON" == "no" ]; then
  echo "No python";
  BUILDPATH="$BUILDPATH-nopython"
  OPTIONS="$OPTIONS -DDISABLE_OPENFLIPPER_PYTHON_SYSTEM=TRUE"
fi

if [ "$IWYU" == "yes" ]; then
  echo "Include what you use enabled.";
  BUILDPATH="$BUILDPATH-iwyu"
  OPTIONS="$OPTIONS -DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
fi

# Not in Docker, so don't mess with the keys

#if test -z "$SSH_PRIVATE_KEY" 
#then
#    echo Skipping ssh environment preparation
#else
#    # Preparing ssh environment
#    echo Prepare ssh environment
#    which ssh-agent || ( echo No SSH Agent found && exit 1 )
#    eval $(ssh-agent -s)
#    echo "$SSH_PRIVATE_KEY" | tr -d '\r' | ssh-add -
#    mkdir -p $HOME/.ssh
#    chmod 700 $HOME/.ssh
#fi

# Don't mess with the repo here.
#if [ ! -d "$HOME/sw/.git" ]; then
#    echo Library update skipped
#else
#  git -C "$HOME/sw" remote add alt https://gitlab-ci-token:${CI_JOB_TOKEN}@gitlab.vci.rwth-aachen.de:9000/Library-Management/Linux-Libraries.git openflipper/Linux-Libraries
#  git -C "$HOME/sw" checkout master
#  git -C "$HOME/sw" pull alt master
#  git -C "$HOME/sw" remote remove alt
#fi

echo "Buildpath is: $BUILDPATH"

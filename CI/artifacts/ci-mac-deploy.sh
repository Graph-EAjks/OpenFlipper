# leave artifacts subdirectory and copy files to toplevel
rsync -a ./artifacts ./
rm -rf artifacts

########################################
# Fetch test data
########################################
rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@www.graphics.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData

#paths should be there as they are artifacts from prior stage
cd build-release

#clean old cmake cache as the path might have changed
find . -name "CMakeCache.txt" -type f -delete

#just to be safe clean the test file definitions too
if [ -f CTestTestfile.cmake ]
then
	rm CTestTestfile.cmake
fi
#just to be safe clean the test file definitions too
if [ -f DartConfiguration.tcl ]
then
	rm DartConfiguration.tcl
fi

# Build without ports to avoid qt4 collision
# fix pathes if run on a different machine
/opt/local/bin/cmake ../ -DCMAKE_CXX_FLAGS='-std=c++11' -DQWT6_INCLUDE_DIR=/Users/jenkins/sw/qwt-6.1.3-qt5.11.3/lib/qwt.framework/Headers -DQWT6_LIBRARY=/Users/jenkins/sw/qwt-6.1.3-qt5.11.3/lib/qwt.framework -DQT5_INSTALL_PATH=/Users/jenkins/sw/Qt/5.11.3/clang_64/   

rm -f *.dmg 

make OpenFlipper_package

mv *.dmg OpenFlipper-Free-GIT-$CI_BUILD_REF-QT5.11.3-CPP11.dmg

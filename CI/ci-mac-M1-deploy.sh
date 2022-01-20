
PATH=$PATH:/opt/homebrew/bin:/opt/local/bin
export PATH

# leave artifacts subdirectory and copy files to toplevel
rsync -a ./artifacts ./
rm -rf artifacts

########################################
# Fetch test data
########################################
rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@gitlab.vci.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData

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
cmake ../ -DQT_VERSION=6    

rm -f *.dmg 

make OpenFlipper_package

mv *.dmg OpenFlipper-Free-GIT-$CI_BUILD_REF-QT6.dmg

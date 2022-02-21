# leave artifacts subdirectory and copy files to toplevel

PATH=$PATH:/opt/homebrew/bin:/opt/local/bin
export PATH

rsync -a ./artifacts ./
rm -rf artifacts

########################################
# Fetch test data
########################################
rm -rf TestData
git clone https://gitlab-ci-token:${CI_JOB_TOKEN}@gitlab.vci.rwth-aachen.de:9000/moebius/OpenFlipper-Test-Data.git TestData

#expect the build-release folder to be present the program binaries are there
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
# run this prior testing to fix paths
cmake ../ -DQT_VERSION=6    


########################################
# Test
########################################

echo "================================"
echo "Current Path is:"
echo "================================"
echo "$PATH"
echo ""


# Run tests
cd tests
/opt/homebrew/bin/python3 run_tests.py

@echo off

::########################################
::# Fetch test data
::########################################
rmdir /Q /S TestData
:: clone libraries git (set env variable to GIT_SSH_COMMAND maybe use setx once as this key won't change) 
set GIT_SSH_COMMAND=ssh -i E:\\\gitlab\\\id_rsa 
git clone git@roosevelt.informatik.rwth-aachen.de:moebius/OpenFlipper-Test-Data.git TestData

::load configuration and settings
call %~dp0\ci-windows-config.bat

echo %Time%: Entering rel directory ...
::enter build directory
cd rel

echo %Time%: Removing CMakeCache ...
::delete cmake cache
del /s /q CMakeCache.txt

echo %Time%: Removing CTestFile ...
del /q CTestTestfile.cmake
echo %Time%: Removing DartConfiguration ...
del /q DartConfiguration.tcl

echo %Time%: running cmake ...
::invoke cmake to fix paths
"C:\Program Files\CMake\bin\cmake.exe"  -DGTEST_PREFIX="%LIBPATH_BASE%\%ARCHITECTURE%\%GTESTVERSION%" -G "%GENERATOR%"  -DCMAKE_BUILD_TYPE=Release -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE %CMAKE_CONFIGURATION% ..

IF %errorlevel% NEQ 0 exit /b %errorlevel%

::run tests
cd tests
copy ..\Build\Qt*.dll testBinaries
copy ..\Build\icu*.dll testBinaries
run_tests.bat

IF %errorlevel% NEQ 0 exit /b %errorlevel%

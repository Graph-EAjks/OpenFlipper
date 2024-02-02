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


echo "CMAKE_CONFIGURATION: %CMAKE_CONFIGURATION%"
ECHO "============================================================="
ECHO "============================================================="
ECHO "Building with :"
whoami
ECHO "ARCHITECTURE        : %ARCHITECTURE%"
ECHO "BUILD_PLATFORM      : %BUILD_PLATFORM%"
ECHO "GTESTVERSION        : %GTESTVERSION%"
ECHO "GENERATOR           : %GENERATOR%"
ECHO "VS_PATH             : %VS_PATH%"
ECHO "LIBPATH             : %LIBPATH%"
ECHO "QT_INSTALL_PATH     : %QT_INSTALL_PATH%"
ECHO "CMAKE_CONFIGURATION : %CMAKE_CONFIGURATION%"
ECHO "============================================================="
ECHO "============================================================="
ECHO ""
ECHO "Running Build environment checks"

IF EXIST %LIBPATH%\ (
  ECHO "LIBPATH ... Ok"
) ELSE (
  ECHO "LIBPATH not found!"
  exit 10;
)


IF EXIST %QT_INSTALL_PATH%\ (
  ECHO "QT_INSTALL_PATH: %QT_INSTALL_PATH% ... Ok"
) ELSE (
  ECHO "QT_INSTALL_PATH: %QT_INSTALL_PATH%\ not found!"
  exit 10;
)


mkdir rel
cd rel

::clear any old binary files
del *.exe

:: Output command line for easier debugging:
echo "C:\Program Files\CMake\bin\cmake.exe"  -DGTEST_PREFIX="%LIBPATH_BASE%/%ARCHITECTURE%/%GTESTVERSION%" -G "%GENERATOR%"  -DCMAKE_BUILD_TYPE=Release -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE %CMAKE_CONFIGURATION% ..

::invoke cmake
"C:\Program Files\CMake\bin\cmake.exe"  -DGTEST_PREFIX="%LIBPATH_BASE%/%ARCHITECTURE%/%GTESTVERSION%" -G "%GENERATOR%"  -DCMAKE_BUILD_TYPE=Release -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE %CMAKE_CONFIGURATION% ..

IF %errorlevel% NEQ 0 exit /b %errorlevel%

:: build Open-Flipper
echo calling: %VS_PATH%
%VS_PATH% /Build "Release" OpenFlipper.sln /Project "ALL_BUILD"

IF %errorlevel% NEQ 0 exit /b %errorlevel%

:: back to the root folder
cd ..

:: copy all files to a new artifacts folder except the .git folder
:: use the following options to make robocopy silent /NFL /NDL /NJH /NJS /nc /ns /np
robocopy . artifacts /e /NFL /NDL /NJH /NJS /nc /ns /np /xd artifacts

::robocopy uses some error codes different from 0 
IF %errorlevel% LSS 8 exit /b 0

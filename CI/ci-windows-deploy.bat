@echo off
::########################################
::# Fetch test data
::########################################
::rmdir /Q /S TestData
:: clone libraries git (set env variable to GIT_SSH_COMMAND maybe use setx once as this key won't change) 
::set GIT_SSH_COMMAND=ssh -i E:\\\gitlab\\\id_rsa 
::git clone git@roosevelt.informatik.rwth-aachen.de:moebius/OpenFlipper-Test-Data.git TestData

::load configuration and settings
call %~dp0\ci-windows-config.bat

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
  ECHO "QT_INSTALL_PATH ... Ok"
) ELSE (
  ECHO "QT_INSTALL_PATH: %QT_INSTALL_PATH%\ not found!"
  exit 10;
)

mkdir rel
cd rel

::remove executable again (no idea why was here already)
del *.exe

::delete cmake cache
del /s /q CMakeCache.txt

:: build installer package dlls and copy them to Openflipper dir

"C:\Program Files\CMake\bin\cmake.exe"  -DGTEST_PREFIX="%LIBPATH_BASE%\%ARCHITECTURE%\%GTESTVERSION%" -G "%GENERATOR%"  -DCMAKE_BUILD_TYPE=Release -DOPENFLIPPER_BUILD_UNIT_TESTS=TRUE %CMAKE_CONFIGURATION% ..

%VS_PATH% /Build "Release" OpenFlipper.sln /Project "PACKAGE"

IF %errorlevel% NEQ 0 exit /b %errorlevel%

move OpenFlipper-*.exe "OpenFlipper-Free-Git-Master-%CI_BUILD_REF%-%BUILD_PLATFORM%-%STRING_ARCH%-%QT_VERSION%.exe"

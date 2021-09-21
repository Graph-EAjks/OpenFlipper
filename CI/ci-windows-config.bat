:: use 4 threads for parallel compilation of the project
set CL=/MP4

:: determine architecture and set variables
if "%ARCHITECTURE%" == "x64" (
set ARCHBITS=_64
set ARCH_VS= Win64
set STRING_ARCH=64-Bit
) else (
set ARCHBITS=
set ARCH_VS=
set STRING_ARCH=32-Bit
)


:: determine VS version and set variables
if "%COMPILER%" == "VS2015" (
set QT_COMPILERPREFIX=msvc2015
set VS_COMPILERVERSION_LONG=14.0
set VS_COMPILERVERSION_SHORT=14
set VS_EDITION_YEAR=2015
set VS_EDITION_PATH= 14.0
)

if "%COMPILER%" == "VS2017" (
set QT_COMPILERPREFIX=msvc2017
set VS_COMPILERVERSION_LONG=15.0
set VS_COMPILERVERSION_SHORT=15
set VS_EDITION_YEAR=2017
::VS2017 default install path is different from other versions
set VS_EDITION_PATH=\2017\Professional
)

if "%COMPILER%" == "VS2019" (
set QT_COMPILERPREFIX=msvc2019
set VS_COMPILERVERSION_LONG=16.0
set VS_COMPILERVERSION_SHORT=16
set VS_EDITION_YEAR=2019
set ARCH_VS=
::VS2017 default install path is different from other versions
set VS_EDITION_PATH=\2019\Professional
)

set BUILD_PLATFORM=%COMPILER%



set GENERATOR=Visual Studio %VS_COMPILERVERSION_SHORT% %VS_EDITION_YEAR%%ARCH_VS%
set VS_PATH="C:\Program Files (x86)\Microsoft Visual Studio%VS_EDITION_PATH%\Common7\IDE\devenv.com"

if "%QT_VERSION%" == "Qt5.10.1" (
set QT_REV_LONG=5.10.1
set QT_REV=5.10.1
)
if "%QT_VERSION%" == "Qt5.13.2" (
set QT_REV_LONG=5.13.2
set QT_REV=5.13.2
)
if "%QT_VERSION%" == "Qt5.14.1" (
set QT_REV_LONG=5.14.1
set QT_REV=5.14.1
)
if "%QT_VERSION%" == "Qt5.14.2" (
set QT_REV_LONG=5.14.2
set QT_REV=5.14.2
)
if "%QT_VERSION%" == "Qt5.15.0" (
set QT_REV_LONG=5.15.0
set QT_REV=5.15.0
)

set QT_SUFFIX=
set QT_BASE_CONFIG=-DQT_INSTALL_PATH=E:\Qt\%QT_REV%\%QT_COMPILERPREFIX%%ARCHBITS%%QT_SUFFIX%


echo "QT config infos :"
echo "QT_VERSION input is : %QT_VERSION%"
echo "QT_REV_LONG :         %QT_REV_LONG%"
echo "QT_REV :              %QT_REV%"
echo "QT Base Config :      %QT_BASE_CONFIG%"

:: set up Libraty Paths
set LIBPATH_BASE=E:/libs/%COMPILER%
set CMAKE_WINDOWS_LIBS_DIR=E:/libs

:: check for gtest version 1.6 or 1.10 use the highest found version
for /l %%x in (6, 1, 10) do (
   if exist %LIBPATH_BASE%/%ARCHITECTURE%/gtest-1.%%x.0 (
      set GTESTVERSION=gtest-1.%%x.0
   )
)

:: check for qwt version 6.1.1 to 6.1.3 use the highest found version
for /l %%x in (0, 1, 6) do (
   echo "Checking for QWT in : %LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt%QT_REV_LONG%/include"
   if exist %LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt%QT_REV_LONG%/include (
      set QWT6_INCLUDE_DIR=%LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt%QT_REV_LONG%/include
      set QWT6_LIBRARY=%LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt%QT_REV_LONG%/lib/qwt.lib
      set QWT6_LIBRARY_DIR=%LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt%QT_REV_LONG%/lib
   )
:: unfortunately qwt is not named consistently in our repos so we also have to check for a different folder named
 echo "Checking for QWT in : %LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt-%QT_REV_LONG%/include"
   if exist %LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt-%QT_REV_LONG%/include (
      set QWT6_INCLUDE_DIR=%LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt-%QT_REV_LONG%/include
      set QWT6_LIBRARY=%LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt-%QT_REV_LONG%/lib/qwt.lib
      set QWT6_LIBRARY_DIR=%LIBPATH_BASE%/%ARCHITECTURE%/qwt-6.1.%%x-qt-%QT_REV_LONG%/lib
   )
)

echo "Python settings for this Job:"
if "%PYTHON%" == "no" (
  echo "Building without python support"
  set CMAKE_CONFIGURATION=%QT_BASE_CONFIG% -DDISABLE_OPENFLIPPER_PYTHON_SYSTEM=TRUE -DQWT6_INCLUDE_DIR=%QWT6_INCLUDE_DIR% -DQWT6_LIBRARY=%QWT6_LIBRARY% -DQWT6_LIBRARY_DIR=%QWT6_LIBRARY_DIR% -DCMAKE_WINDOWS_LIBS_DIR=%CMAKE_WINDOWS_LIBS_DIR% 
) else (
  echo "Build with python support"
  set CMAKE_CONFIGURATION=%QT_BASE_CONFIG% -DQWT6_INCLUDE_DIR=%QWT6_INCLUDE_DIR% -DQWT6_LIBRARY=%QWT6_LIBRARY% -DQWT6_LIBRARY_DIR=%QWT6_LIBRARY_DIR% -DCMAKE_WINDOWS_LIBS_DIR=%CMAKE_WINDOWS_LIBS_DIR%
)

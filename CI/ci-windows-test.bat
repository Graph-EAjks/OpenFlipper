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

ECHO "============================================================="
ECHO "============================================================="
ECHO "Building as user :"
whoami

"C:\Program Files\Python39\python.exe" rel\tests\run_tests.py

IF %errorlevel% NEQ 0 exit /b %errorlevel%

echo %Time%: Python ctest runner executed successfully with errorlevel: %errorlevel%
echo %Time%: At working directory: %CD%

cd ..
cd ..


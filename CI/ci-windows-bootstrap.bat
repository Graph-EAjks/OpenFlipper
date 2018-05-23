@echo off

REM bootstrapping script for stage 2 ci scripts
REM to avoid repetitive cloning, in lieu of the sgitlab tickyrunners
REM we copy the entire content of our bild folder to an artifacts subdolder 
REM and share it with other buildstages.
REM
REM initial building is considered as stage 0,this script is stage 1 all 
REM following stages are stage 2. The purpos of this script is to move all 
REM files from the artifacts directory back to the root directory and call 
REM the script for this ci job.
REM This way the stage 2 script can delete the artifacts folder safely, as
REM deleting a script that is running will cause an error on windows systems.
REM 

REM #######################################
REM # bootstrap level 0 
REM #######################################




IF "%BOOTSTRAPLEVEL%" == "" ( 
set BOOTSTRAPLEVEL=0
)

IF "%BOOTSTRAPLEVEL%" == "0" (

REM copy all files from artifacts folder to root folder
echo %TIME%: Restoring artifacts from artifacts directory ...
robocopy artifacts ./ /e /NFL /NDL /NJH /NJS /nc /ns /np

REM set the bootstrap level to 1
echo %TIME%: Entering bootstrap level 1 ...
set BOOTSTRAPLEVEL=1

REM call the copy of this script outside artifacts folder
call %~dp0\..\..\CI\ci-windows-bootstrap.bat

REM return the error code if not 0
IF "%errorlevel%" NEQ "0" exit /b %errorlevel%
) else (

REM #######################################
REM # bootstrap level 1
REM #######################################
REM remove the artifacts folder, it may cause problems with e.g. cmake
echo %Time%: Removing artifacts directory ...
rmdir /Q /S artifacts

REM call the stage 2 script
echo %Time%: Calling %~dp0\%STAGE2SCRIPT% ...
call %~dp0\%STAGE2SCRIPT%

REM store the error code of the stage 2 script
set bootstrapretval=%errorlevel%

REM copy this script back to the artifacts folder 
if not exist ./artifacts (
echo %Time%: recrerating artifacts directory ...
cd %~dp0\..
mkdir artifacts
cd artifacts
mkdir CI
cd ..
)
echo %Time%: restoring bootstrap script ...
copy %~dp0\ci-windows-bootstrap.bat artifacts\CI\ci-windows-bootstrap.bat

REM when this script returns, the windows Command interpreter will look
REM at the original position for the script to continue after the the call command
REM of level 0
IF "%bootstrapretval%" NEQ "0" exit /b %bootstrapretval%
)
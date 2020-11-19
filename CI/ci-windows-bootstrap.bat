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

echo %Time%: Bootstrap script line 1 at level 0 with working directory %CD% and current error level %errorlevel% ...

REM copy all files from artifacts folder to root folder
echo %TIME%: Restoring artifacts from artifacts directory ...
robocopy artifacts ./ /e /NFL /NDL /NJH /NJS /nc /ns /np

REM set the bootstrap level to 1
echo %Time%: Returning after robocopy with working directory %CD% with current error level %errorlevel% ...

cmd /c "exit /b 0"

echo %Time%: Preparing to enter Bootstrap level 1 from working directory %CD% with current error level %errorlevel% ...

echo %Time%: Now calling %~dp0..\..\CI\ci-windows-bootstrap.bat

REM call the copy of this script outside artifacts folder
call  %~dp0..\..\CI\ci-windows-bootstrap-level1.bat

REM return the error code if not 0
IF "%errorlevel%" NEQ "0" exit /b %errorlevel%

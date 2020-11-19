@echo off

echo %Time%: Bootstrap level 1 at working directory %CD% with current error level %errorlevel% ...

REM #######################################
REM # bootstrap level 1
REM #######################################
REM remove the artifacts folder, it may cause problems with e.g. cmake

echo %Time%: Bootstrap level 1 at working directory %CD% with current error level %errorlevel% ...

echo %Time%: Removing artifacts directory ...
rmdir /Q /S artifacts

echo %Time%: Bootstrap level 1 at working directory %CD% with current error level %errorlevel% ...

REM call the stage 2 script
echo %Time%: Bootstrap level 1 at working directory %CD% with current error level %errorlevel% ...

echo %Time%: Calling %~dp0%STAGE2SCRIPT% from working directory %CD% with current error level %errorlevel% ...

call %~dp0%STAGE2SCRIPT%

echo %Time%: Stage 2 returned with error level: %errorlevel% at working directory: %CD%

IF %errorlevel% NEQ 0 exit /b %errorlevel%

echo %Time%: Stage 2 script executed successfully

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
exit /b %bootstrapretval%



@ECHO OFF

REM %1 == The name of the project that called the custom build step
REM %2 == HAVOK_TOOLS_ROOT (taken from environment variable)
REM %3 == TARGET_PATH (full path to DLL)

if "%~2" == "" goto failure

REM Save re-execution details in redeploy.bat
set REDEPLOY_FILE=%~dp0redeploy_%~n1.bat
if exist %REDEPLOY_FILE% del %REDEPLOY_FILE%
echo %~dpnx0 %* > %REDEPLOY_FILE%

setlocal
mkdir "%~2\filters" > nul
xcopy "%~3" "%~2\filters" /s /y /r >nul
if errorlevel 1 goto failure

echo Files copied to %2

REM We copy the output to a file so VisualStudio knows that the build step was successful

echo Files copied to %2 > "%~3.output"

exit /b 0

endlocal 


:failure

echo Couldn't copy files to %2... 
echo Make sure DLLs are not in use and that HAVOK_TOOLS_ROOT is defined

REM Print an error message for the continuous integration system to detect.
echo ERROR: Custom build step failed.

REM Return an ErrorLevel of 1
exit /b 1


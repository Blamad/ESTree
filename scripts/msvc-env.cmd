@echo off
rem Runs a command inside an MSVC x64 developer environment (needed by the ninja-msvc preset).
rem Usage: scripts\msvc-env.cmd cmake --preset ninja-msvc
rem Override the environment script with VCVARS64 (full path to vcvars64.bat).
setlocal
if "%VCVARS64%"=="" set "VCVARS64=%ProgramFiles(x86)%\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS64%" goto :missing
call "%VCVARS64%" >nul || exit /b 1
%*
exit /b %ERRORLEVEL%

:missing
rem no parenthesized block here: the expanded path contains "(x86)"
echo vcvars64.bat not found: %VCVARS64%
echo Set VCVARS64 to your Visual Studio 2019 or newer vcvars64.bat.
exit /b 1

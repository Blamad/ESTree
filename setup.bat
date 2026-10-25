@echo off
rem Configures the project with a CMake preset and links Resources next to the binaries.
rem Usage: setup.bat [preset] [--clean]      (default preset: vs2019; see CMakePresets.json)
rem Requires CMake >= 3.21 on PATH and a Visual Studio C++ toolset; there are no other prerequisites.
setlocal
set PRESET=vs2019
set CLEAN=0
for %%A in (%*) do (
	if "%%A"=="--clean" (set CLEAN=1) else (set PRESET=%%A)
)

if "%CLEAN%"=="1" if exist "build\%PRESET%" rmdir /s /q "build\%PRESET%"

cmake --preset %PRESET% || exit /b 1

if not exist "build\%PRESET%\Resources" mklink /d "build\%PRESET%\Resources" "%~dp0Resources"
endlocal

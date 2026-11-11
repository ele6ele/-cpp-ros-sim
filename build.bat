@echo off
rem ================================================================
rem  Build script (Windows + MSVC)
rem  Usage: double-click, or run "build.bat", or Ctrl+Shift+B in VS Code
rem  What it does:
rem    1) locate Visual Studio via vswhere
rem    2) load the MSVC environment (vcvars64.bat)
rem    3) compile every .cpp in src\ into build\ros_sim.exe
rem ================================================================
setlocal enabledelayedexpansion
pushd "%~dp0"

set "VCVARS="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VCVARS=%%i\VC\Auxiliary\Build\vcvars64.bat"
)
if not defined VCVARS goto :no_vs
if not exist "%VCVARS%" goto :no_vs

call "%VCVARS%" >nul 2>&1
if errorlevel 1 goto :no_vs

if not exist "build" mkdir "build"

set "SRC="
for %%f in ("src\*.cpp") do set "SRC=!SRC! "%%~ff""
if not defined SRC (
    echo [ERROR] No .cpp file found in src\
    popd & exit /b 1
)

echo [build] compiling ...
cl /nologo /std:c++17 /EHsc /utf-8 /W4 /permissive- /Zi ^
   /Iinclude /Fo:build\ /Fd:build\vc.pdb /Fe:build\ros_sim.exe !SRC! /DEBUG

if errorlevel 1 (
    echo.
    echo [FAILED] compile error, see messages above
    popd & exit /b 1
)

echo [build] OK -^> build\ros_sim.exe
popd
exit /b 0

:no_vs
echo [ERROR] MSVC environment not found.
echo         Install Visual Studio with the "Desktop development with C++" workload,
echo         or edit the vcvars64.bat path inside this script.
popd
exit /b 1

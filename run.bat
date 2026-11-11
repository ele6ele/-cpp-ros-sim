@echo off
rem Build if needed, then run the program (console set to UTF-8 so Chinese logs show correctly)
pushd "%~dp0"

if not exist "build\ros_sim.exe" call "%~dp0build.bat"
if not exist "build\ros_sim.exe" exit /b 1

chcp 65001 >nul
"build\ros_sim.exe" %*
set "CODE=%ERRORLEVEL%"
popd
exit /b %CODE%

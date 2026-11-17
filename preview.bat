@echo off
rem Run the program, render logs\run.log into logs\report.html, then open it in the browser.
pushd "%~dp0"

if not exist "build\ros_sim.exe" call "%~dp0build.bat"
if not exist "build\ros_sim.exe" exit /b 1

echo [preview] running ros_sim.exe (log -> logs\run.log) ...
if not exist "logs" mkdir "logs"
del /q "logs\run.log" >nul 2>&1
"build\ros_sim.exe" >nul

echo [preview] rendering report ...
powershell -NoProfile -ExecutionPolicy Bypass -File "tools\make_report.ps1"
if errorlevel 1 (
    echo [preview] failed to build report
    popd & exit /b 1
)

echo [preview] opening logs\report.html ...
start "" "logs\report.html"
popd
exit /b 0

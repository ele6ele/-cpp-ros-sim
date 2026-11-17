@echo off
rem Run the program, render logs\run.log into logs\report.html (+ report.png), then open it.
pushd "%~dp0"

if not exist "build\ros_sim.exe" call "%~dp0build.bat"
if not exist "build\ros_sim.exe" exit /b 1

echo [preview] running ros_sim.exe, log goes to logs\run.log ...
if not exist "logs" mkdir "logs"
del /q "logs\run.log" >nul 2>&1
"build\ros_sim.exe" >nul

echo [preview] rendering report ...
powershell -NoProfile -ExecutionPolicy Bypass -File "tools\make_report.ps1"
if errorlevel 1 (
    echo [preview] failed to build report
    popd & exit /b 1
)

set "EDGE=%ProgramFiles(x86)%\Microsoft\Edge\Application\msedge.exe"
set "CHROME=%ProgramFiles%\Google\Chrome\Application\chrome.exe"
set "URL=file:///%CD:\=/%/logs/report.html"

rem 用无头浏览器另存一张 PNG，方便在没有浏览器时直接看
if exist "%EDGE%" (
    echo [preview] rendering logs\report.png ...
    "%EDGE%" --headless=new --disable-gpu --hide-scrollbars --window-size=1300,2300 --screenshot="%CD%\logs\report.png" "%URL%" >nul 2>&1
)

echo [preview] opening report ...
if exist "%EDGE%" (
    start "" "%EDGE%" "%URL%"
) else if exist "%CHROME%" (
    start "" "%CHROME%" "%URL%"
) else (
    start "" "logs\report.html"
)

popd
exit /b 0

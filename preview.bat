@echo off
rem 运行程序 -> 生成可视化报告 -> 用默认浏览器打开
pushd "%~dp0"

if not exist "build\ros_sim.exe" call "%~dp0build.bat"
if not exist "build\ros_sim.exe" exit /b 1

echo [preview] 运行程序并写入 logs\run.log ...
if not exist "logs" mkdir "logs"
del /q "logs\run.log" >nul 2>&1
"build\ros_sim.exe" >nul

echo [preview] 生成可视化报告 ...
powershell -NoProfile -ExecutionPolicy Bypass -File "tools\make_report.ps1"
if errorlevel 1 (
    echo [preview] 报告生成失败
    popd & exit /b 1
)

start "" "logs\report.html"
popd
exit /b 0

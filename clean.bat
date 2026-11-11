@echo off
rem Remove build artifacts
pushd "%~dp0"
if exist "build" (
    rmdir /s /q "build"
    echo [clean] build\ removed
) else (
    echo [clean] nothing to do
)
popd

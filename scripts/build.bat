@echo off
setlocal

:: Check if VCPKG_ROOT is set, otherwise use default path
if not defined VCPKG_ROOT set VCPKG_ROOT=C:\vcpkg

set TOOLCHAIN=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake

:: Check if toolchain file exists
if not exist "%TOOLCHAIN%" (
    echo Error: vcpkg toolchain file not found at %TOOLCHAIN%
    exit /b 1
)

:: Run CMake
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN%"
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

cmake --build build
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

echo Build completed successfully.
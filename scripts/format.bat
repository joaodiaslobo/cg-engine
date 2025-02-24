@echo off
setlocal enabledelayedexpansion

set CLANG_FORMAT=clang-format

where %CLANG_FORMAT% >nul 2>nul
if %errorlevel% neq 0 (
    echo clang-format not found! Please install it or add it to your PATH.
    exit /b 1
)

for /r engine %%f in (*.cpp *.hpp) do (
    echo Formatting %%f
    %CLANG_FORMAT% -i "%%f"
)

for /r generator %%f in (*.cpp *.hpp) do (
    echo Formatting %%f
    %CLANG_FORMAT% -i "%%f"
)

echo Formatting complete!
endlocal
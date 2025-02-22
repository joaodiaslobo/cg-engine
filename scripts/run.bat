@echo off
setlocal

:: Set default application to "engine" if no argument is provided
set "APP=%~1"
if "%APP%"=="" set "APP=engine"

:: Convert argument to lowercase (for case insensitivity)
for %%A in (generator engine) do (
    if /I "%APP%"=="%%A" set "APP=%%A"
)

:: Shift the first argument to remove it from the argument list
shift

:: Rebuild the remaining arguments manually
set "ARGS="
:loop
if "%~1"=="" goto run_app
set "ARGS=%ARGS% %1"
shift
goto loop

:run_app
:: Set executable paths
set "BUILD_DIR=build"
set "GENERATOR_EXE=%BUILD_DIR%\generator\Debug\generator.exe"
set "ENGINE_EXE=%BUILD_DIR%\engine\Debug\engine.exe"

:: Run the selected application with the remaining arguments
if "%APP%"=="generator" (
    if exist "%GENERATOR_EXE%" (
        "%GENERATOR_EXE%" %ARGS%
    ) else (
        echo Error: generator.exe not found in %BUILD_DIR%
        exit /b 1
    )
) else if "%APP%"=="engine" (
    if exist "%ENGINE_EXE%" (
        "%ENGINE_EXE%" %ARGS%
    ) else (
        echo Error: engine.exe not found in %BUILD_DIR%
        exit /b 1
    )
) else (
    echo Invalid option: %APP%
    echo Usage: %~nx0 [generator^|engine] [additional arguments...]
    exit /b 1
)

endlocal

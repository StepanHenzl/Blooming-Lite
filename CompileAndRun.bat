@echo off
setlocal

cd /d %~dp0

REM root folder
set "BASE_DIR=%~dp0"

REM activate emsdk
call "%BASE_DIR%dependencies\emsdk-main\emsdk_env.bat"

where emcc
if errorlevel 1 (
    echo Please run InstallEmscripten and try again.
    pause
    exit /b 1
)

call emcmake cmake -S . -B build
cd build
call emmake make

rem Open main engine file in browser.
start "" "http://localhost:8080/index.html"
rem Run local server
emrun --no_browser --port 8080 build



@echo off
setlocal

set "BASE_DIR=%~dp0"
set "INSTALL_DIR=%BASE_DIR%dependencies"

echo Downloading emscripten files:

mkdir "%INSTALL_DIR%" 2>nul
cd /d "%INSTALL_DIR%"

powershell -Command ^
"Invoke-WebRequest https://github.com/emscripten-core/emsdk/archive/refs/heads/main.zip -OutFile emsdk.zip"

powershell -Command ^
"Expand-Archive -Force emsdk.zip ."

for /d %%i in (emsdk-*) do set EMSDK_FOLDER=%%i

cd "%EMSDK_FOLDER%"

set "PYTHON=%BASE_DIR%dependencies\python\3.13.3_64bit\python.exe"
set "PATH=%BASE_DIR%dependencies\python\3.13.3_64bit;%PATH%"

echo Activating EMSDK:
call emsdk_env.bat

echo Installing EMSDK toolchain:
call emsdk.bat install latest
call emsdk.bat activate latest

echo Huray we are DONE. DONE. DONE!!!! (hopefully)
pause

endlocal
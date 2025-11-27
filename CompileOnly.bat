@echo off

rem Checks if Emscripten is available.
where emcc >nul 2>nul
if errorlevel 1 ( 
	echo Please add Emscripten to your PATH.
	exit /b 1 
)

call emcmake cmake -S . -B build
cd build
call emmake make



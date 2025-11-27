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

rem Open main engine file in edge.
start "" "C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" "http://localhost:8080/index.html"
rem Run local server
emrun --no_browser --port 8080 build



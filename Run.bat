rem @echo off

cd /d %~dp0
cd build

rem Open main engine file in edge.
start "" "http://localhost:8080/index.html"
rem Run local server
..\dependencies\busybox.exe httpd -f -p 8080

pause


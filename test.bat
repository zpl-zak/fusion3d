@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
devenv "build\Fusion3D.sln" /Build "Release"
"build\Release\%1.exe"
pause
cls
color 07
@echo on
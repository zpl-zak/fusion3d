@echo off

set LIBS="kernel32.lib legacy_stdio_definitions.lib user32.lib gdi32.lib opengl32.lib"

if not exist build mkdir build
pushd build
../msvc.bat ../code/win32_fusion3d.c D %LIBS%
popd


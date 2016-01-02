@ECHO OFF

IF "%1" == "build" GOTO BUILD
IF "%1" == "test" GOTO TEST
IF "%1" == "debug" GOTO DEBUG
IF "%1" == "clean" GOTO CLEAN
IF "%1" == "copyall" GOTO COPYALL
IF "%1" == "copydata" GOTO COPYDATA
GOTO ERROR

:BUILD
CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
DEVENV "build\Fusion3D.sln" /Build "Release"
GOTO END

:TEST
START /b "%1" "build\Release\%2.exe"
GOTO END

:DEBUG
#CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\bin\vcvars32.bat"
#DEVENV "build\Fusion3D.sln" /Build "Debug"
"build\Debug\%2.exe"
GOTO END

:CLEAN
RMDIR /S /Q "build/Release"
RMDIR /S /Q "build/Debug"
CLS
GOTO END

:COPYALL
XCOPY  /D /y /s /r lib\_bin build\Release
XCOPY  /Y /I /E data\*.* build\Release\data
XCOPY  /Y /I /E python\*.* build\Release\python

XCOPY  /D /y /s /r lib\_bin build\Debug
XCOPY  /Y /I /E data\*.* build\Debug\data
XCOPY  /Y /I /E python\*.* build\Debug\python
GOTO END

:COPYDATA
XCOPY  /Y /I /E data\*.* build\Release\data
XCOPY  /Y /I /E data\*.* build\Debug\data
GOTO END

:ERROR
ECHO UNKNOWN OPTION: "%1"
GOTO END

:END
COLOR 07
@ECHO ON
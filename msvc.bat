@echo off

IF NOT DEFINED clset (call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x64)
SET clset=64

SET SRC=%1
if "%SRC%" == "" SET SRC=main.cpp

SET ADD=%2
SET PAL=%~3
SET LIBS=%~4

SET WARN=-D_CRT_SECURE_NO_WARNINGS /wd4189 /wd4310 /wd4100 /wd4201 /wd4505 /wd4996 /wd4127 /wd4510 /wd4512 /wd4610 /wd4457
SET OPTS=%WARN% /W3 /WX /GR- /nologo -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi /GS- /Gs9999999

SET DEBUG=/Zi /Zo /DEBUG
SET OPTI=/FC -O2
SET O=%OPTI%
if "%ADD%" == "D" SET O=%DEBUG%

SET CODE_HOME=%~dp0
SET "HFTW_CODE=\code"
SET HFTW_PATH=%CODE_HOME%libs\hftw%HFTW_CODE%

ctime -begin %SRC%.ctm
cl /I%CODE_HOME% /I%HFTW_PATH% %OPTS% %O% %SRC% /link %PAL% -stack:0x100000,0x100000 %LIBS% /incremental:no -opt:ref
ctime -end %SRC%.ctm %ERRORLEVEL%

rem del *.obj
rem del *.exp
rem del *.ilk

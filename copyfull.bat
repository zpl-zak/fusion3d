xcopy  /D /y /s /r lib\_bin build\Release
xcopy  /Y /I /E data\*.* build\Release\data
xcopy  /Y /I /E python\*.* build\Release\python

xcopy  /D /y /s /r lib\_bin build\Debug
xcopy  /Y /I /E data\*.* build\Debug\data
xcopy  /Y /I /E python\*.* build\Debug\python
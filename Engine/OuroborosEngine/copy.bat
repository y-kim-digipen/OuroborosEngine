
xcopy /y /d "%cd%\external\bin"  "%cd%\build\Release\"
xcopy /y /d "%cd%\external\lua-5.4.2\lua54.dll"  "%cd%\build\Release\"
xcopy /y /d /e /i "%cd%\shaders" "%cd%\build\Release\shaders\"
xcopy /y /d /e /i "%cd%\model" "%cd%\build\Release\model\"
xcopy /y /d /e /i "%cd%\images" "%cd%\build\Release\images\"
xcopy /y /d /e /i "%cd%\scripts" "%cd%\build\Release\scripts\"


xcopy /y /d "%cd%\external\bin"  "%cd%\build\Debug\"
xcopy /y /d "%cd%\external\lua-5.4.2\lua54.dll"  "%cd%\build\Debug\"
xcopy /y /d /e /i "%cd%\shaders" "%cd%\build\Debug\shaders\"
xcopy /y /d /e /i "%cd%\model" "%cd%\build\Debug\model\"
xcopy /y /d /e /i "%cd%\images" "%cd%\build\Debug\images\"
xcopy /y /d /e /i "%cd%\scripts" "%cd%\build\Debug\scripts\"

xcopy /y /d /e /i "%cd%\shaders" "%cd%\build\shaders\"
xcopy /y /d /e /i "%cd%\model" "%cd%\build\model\"
xcopy /y /d /e /i "%cd%\images" "%cd%\build\images\"
xcopy /y /d /e /i "%cd%\scripts" "%cd%\build\scripts\"
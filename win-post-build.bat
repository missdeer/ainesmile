IF NOT EXIST "%APPDATA%\DForD Software\ainesmile" mkdir "%APPDATA%\DForD Software\ainesmile"
copy resource\.ainesmilerc "%APPDATA%\DForD Software\ainesmile\.ainesmilerc"
copy resource\*.xml "%APPDATA%\DForD Software\ainesmile\"
IF NOT EXIST "%APPDATA%\DForD Software\ainesmile\language" mkdir "%APPDATA%\DForD Software\ainesmile\language"
xcopy /S /E /Y resource\language "%APPDATA%\DForD Software\ainesmile\language"
IF NOT EXIST "%APPDATA%\DForD Software\ainesmile\themes" mkdir "%APPDATA%\DForD Software\ainesmile\themes"
xcopy /S /E /Y resource\themes  "%APPDATA%\DForD Software\ainesmile\themes"
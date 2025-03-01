@echo off
echo Add to start menu (y/n)?
set /p sm=
echo Add to desktop (y/n)?
set /p dsk=
if %sm% == n goto chkdsk
::mklink MassFollower %~dp0MassFollower.exe
cd "%AppData%\Microsoft\Windows\Start Menu\Programs"
mkdir frostsoftware
del "%AppData%\Microsoft\Windows\Start Menu\Programs\frostsoftware\MassFollower.url"
::move %~dp0MassFollower "%AppData%\Microsoft\Windows\Start Menu\Programs\frostsoftware"
echo [InternetShortcut] >> "%AppData%\Microsoft\Windows\Start Menu\Programs\frostsoftware\MassFollower.url"
echo URL=%~dp0\MassFollower.exe >> "%AppData%\Microsoft\Windows\Start Menu\Programs\frostsoftware\MassFollower.url"
echo IconFile=%~dp0\asset\wasteoffollowerlogo.ico >> "%AppData%\Microsoft\Windows\Start Menu\Programs\frostsoftware\MassFollower.url"
echo IconIndex=0 >> "%AppData%\Microsoft\Windows\Start Menu\Programs\frostsoftware\MassFollower.url"
cd %~dp0
echo Installed Start Menu
:chkdsk
if %dsk% == n goto end
del "%userprofile%\desktop\MassFollower.url"
echo [InternetShortcut] >> "%userprofile%\desktop\MassFollower.url"
echo URL=%~dp0\MassFollower.exe >> "%userprofile%\desktop\MassFollower.url"
echo IconFile=%~dp0\asset\wasteoffollowerlogo.ico >> "%userprofile%\desktop\MassFollower.url"
echo IconIndex=0 >> "%userprofile%\desktop\MassFollower.url"
echo Installed Desktop
:end
pause
exit
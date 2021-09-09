echo off

REM is a command used to express comments, those lines will not be executed

REM ----------------------------------------------------------------------------------------------

REM Add the secube program directory to PATH, in order to run secube_cmd.exe from everywhere
setx path "%path%;%ProgramFiles%\secube"


REM -------------------------------------------------------------------------------------------

REM Elevate privileges

set "params=%*"
cd /d "%~dp0" && ( if exist "%temp%\getadmin.vbs" del "%temp%\getadmin.vbs" ) && fsutil dirty query %systemdrive% 1>nul 2>nul || (  echo Set UAC = CreateObject^("Shell.Application"^) : UAC.ShellExecute "cmd.exe", "/k cd ""%~sdp0"" && %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs" && "%temp%\getadmin.vbs" && exit /B )

REM ---------------------------------------------------------------------------------------------

REM Copy executables in Program Files directory

mkdir "%ProgramFiles%\secube"
xcopy binaries "%ProgramFiles%\secube\" /E /H /C /I
 
REM ----------------------------------------------------------------------------------------------

REM Install the shortcut in the contextual menu

REG ADD "HKEY_CLASSES_ROOT\*\shell\SEcube\command" /d "%ProgramFiles%\secube\secube_utilities.exe %%1"

REM -----------------------------------------------------------------------------------------------

REM Add the new SEcube folder to the PATH variable

set PATH=%PATH%;%ProgramFiles%\secube\

REM -----------------------------------------------------------------------------------------------

echo Installation completed

REM Installation completed

pause



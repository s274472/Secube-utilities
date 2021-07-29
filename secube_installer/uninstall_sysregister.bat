echo off

REM is a command used to express comments, those lines will not be executed

REM -------------------------------------------------------------------------------------------

REM Elevate privileges

set "params=%*"
cd /d "%~dp0" && ( if exist "%temp%\getadmin.vbs" del "%temp%\getadmin.vbs" ) && fsutil dirty query %systemdrive% 1>nul 2>nul || (  echo Set UAC = CreateObject^("Shell.Application"^) : UAC.ShellExecute "cmd.exe", "/k cd ""%~sdp0"" && %~s0 %params%", "", "runas", 1 >> "%temp%\getadmin.vbs" && "%temp%\getadmin.vbs" && exit /B )

REM ---------------------------------------------------------------------------------------------

REM Delete executables in Program Files directory

del /s /q "%ProgramFiles%\secube"
rmdir "%ProgramFiles%\secube" /s /q

REM ----------------------------------------------------------------------------------------------

REM Delete the shortcut in the contextual menu

REG DELETE "HKEY_CLASSES_ROOT\*\shell\SEcube" /f

REM -----------------------------------------------------------------------------------------------

echo Uninstall complete

pause



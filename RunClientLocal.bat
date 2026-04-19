@echo off
setlocal

set "ProjectRoot=%~dp0"
set "ProjectFile=%ProjectRoot%Overwatch.uproject"
set "EditorExe=F:\UE_5.7.4\Engine\Binaries\Win64\UnrealEditor-Win64-DebugGame.exe"
set "ServerAddress=127.0.0.1:7777"

if not exist "%ProjectFile%" (
	echo [ERROR] Project file not found:
	echo %ProjectFile%
	echo.
	pause
	exit /b 1
)

if not exist "%EditorExe%" (
	echo [ERROR] Editor executable not found:
	echo %EditorExe%
	echo.
	echo Update the engine path in this bat file if needed.
	pause
	exit /b 1
)

pushd "%ProjectRoot%"
"%EditorExe%" "%ProjectFile%" %ServerAddress% -game -log -windowed -ResX=1600 -ResY=900
popd

endlocal

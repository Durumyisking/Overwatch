@echo off
setlocal

set "ProjectRoot=%~dp0"
set "ProjectFile=%ProjectRoot%Overwatch.uproject"
set "RunUAT=F:\UE_5.7.4\Engine\Build\BatchFiles\RunUAT.bat"

if not exist "%ProjectFile%" (
	echo [ERROR] Project file not found:
	echo %ProjectFile%
	echo.
	pause
	exit /b 1
)

if not exist "%RunUAT%" (
	echo [ERROR] RunUAT.bat not found:
	echo %RunUAT%
	echo.
	echo Update the engine path in this bat file if needed.
	pause
	exit /b 1
)

pushd "%ProjectRoot%"
call "%RunUAT%" BuildCookRun -project="%ProjectFile%" -noP4 -build -cook -stage -pak -server -noclient -serverplatform=Win64 -serverconfig=Development -allmaps -utf8output
popd

endlocal

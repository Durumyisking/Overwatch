@echo off
setlocal

set "ProjectRoot=%~dp0"
set "ServerMap=/Game/Overwatch/Level/Lobby"
set "ServerPort=7777"
set "StageRoot=%ProjectRoot%Saved\StagedBuilds"
set "ServerExe="

for /f "delims=" %%I in ('dir /s /b "%StageRoot%\OverwatchServer.exe" 2^>nul') do (
	set "ServerExe=%%I"
	goto :FoundServerExe
)

:FoundServerExe
if not defined ServerExe (
	echo [ERROR] Staged dedicated server executable not found.
	echo.
	echo Run BuildCookStageDedicatedServer.bat first.
	echo Expected search root:
	echo %StageRoot%
	echo.
	pause
	exit /b 1
)

pushd "%ProjectRoot%"
"%ServerExe%" %ServerMap% -log -port=%ServerPort%
popd

endlocal

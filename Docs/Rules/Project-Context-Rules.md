# Project Context Rules

## Repository Contract
- This is an Unreal Engine project.
- Treat the repository root as the working root and prefer the local `.uproject` as the main project root when present.
- Preserve determinism, runtime consistency, and restorable state.

## Project Priorities
- Performance, determinism, and reproducibility come first.
- Preserve save/load correctness across Combat and BoardGame flows.
- Do not reset domain state when a flow transition only requires unload/reload.
- Keep ownership explicit: who owns, mutates, serializes, restores, and invalidates each runtime datum.

## Project Commands
- Build example:
  `Engine/Build/BatchFiles/Build.bat <ProjectEditorTarget> Win64 Development "<Project>.uproject" -WaitMutex`
- Automation test example:
  `"<UEEditor-Cmd.exe>" "<Project>.uproject" -ExecCmds="Automation RunTests <Filter>; Quit" -unattended -nop4 -nosplash`

## Suggested Reading Order
- Inspect `Source/` first.
- Inspect `Config/` second.
- Check project notes/docs third.
- Broaden only after checking the directly affected module.

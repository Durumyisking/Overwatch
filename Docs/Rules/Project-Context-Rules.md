# Project Context Rules

## Repository Contract
- This is an Unreal Engine Overwatch-style study project.
- The current learning focus is dedicated server, GAS, CommonUI, MVVM, and Lyra-style modular initialization.
- Treat the repository root as the working root and prefer the local `.uproject` as the main project root when present.
- Preserve deterministic multiplayer behavior, runtime consistency, and explicit gameplay state ownership.

## Project Priorities
- Server-authoritative gameplay comes first.
- Keep GAS ownership, AvatarActor, OwnerActor, ability grants, input binding, and respawn flow explicit.
- Keep CommonUI and MVVM in presentation and input-routing roles only.
- Prefer Lyra/Hak-style Experience, GameStateComponent, GameFrameworkComponentManager, and GameFeature driven setup when it fits the current slice.
- Start from the smallest playable multiplayer slice before adding broad live-service systems.
- Keep ownership explicit: who owns, mutates, serializes, restores, and invalidates each runtime datum.

## Project Commands
- Editor build:
  `F:\UE_5.7.4\Engine\Build\BatchFiles\Build.bat OverwatchEditor Win64 Development "F:\Dev\Overwatch\Overwatch.uproject" -WaitMutex`
- Server build:
  `F:\UE_5.7.4\Engine\Build\BatchFiles\Build.bat OverwatchServer Win64 Development "F:\Dev\Overwatch\Overwatch.uproject" -WaitMutex`
- Automation test example:
  `"<UnrealEditor-Cmd.exe>" "F:\Dev\Overwatch\Overwatch.uproject" -ExecCmds="Automation RunTests <Filter>; Quit" -unattended -nop4 -nosplash`

## Suggested Reading Order
- Inspect `Source/` first.
- Inspect `Config/` second.
- Check `Docs/Research/` only when the task touches Lyra adoption or Overwatch gameplay direction.
- Broaden only after checking the directly affected module.

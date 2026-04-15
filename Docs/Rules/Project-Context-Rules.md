# Project Context Rules

## Repository Contract
- This is an Unreal Engine 5.7.3 C++ project rooted at `Overwatch.uproject`.
- Treat the repository root as the working root.
- Prefer local, compile-safe changes that preserve explicit ownership and multiplayer correctness.

## Learning Goals
- Dedicated Server
- CommonUI
- GAS
- MVVM

## Project Priorities
- Server authority first.
- Clear ownership and one source of truth per runtime datum.
- UI and gameplay logic separation.
- Dedicated-server-safe initialization and flow control.
- Learning-friendly architecture over tutorial shortcuts.

## Build And Verification Commands
- Editor build:
  `F:\UE_5.7\Engine\Build\BatchFiles\Build.bat OverwatchEditor Win64 Development "F:\Dev\Overwatch\Overwatch.uproject" -WaitMutex`
- If a dedicated server target is added later, verify server builds separately instead of assuming editor success proves server safety.
- Automation test pattern:
  `"<UEEditor-Cmd.exe>" "F:\Dev\Overwatch\Overwatch.uproject" -ExecCmds="Automation RunTests <Filter>; Quit" -unattended -nop4 -nosplash`

## Suggested Reading Order
- Inspect the directly affected module in `Source/` first.
- Check related config in `Config/` second.
- Read local docs third.
- Broaden only after confirming the current owner and authority path.

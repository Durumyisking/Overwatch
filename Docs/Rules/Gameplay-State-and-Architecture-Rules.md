# Gameplay State And Architecture Rules

## Architecture Rules
- Server authority is the default for gameplay state changes.
- Managers / Flows / Subsystems orchestrate sequencing, transitions, and assembly only.
- Domain objects own domain rules, state transitions, serialization, and restore logic.
- Prefer interfaces, base classes, delegates, factories, and registries over repeated cast-based branching.
- Prefer adding new types or override points over extending large conditional blocks.
- 예외 처리용 가상함수나 옵션 bool을 새로 만들기 전에, 자료구조/상태머신/도메인 객체로 흡수할 수 있는지 먼저 검토해.
- Keep one authoritative owner per gameplay datum.
- Avoid duplicated truth across GameMode, GameState, PlayerState, Controller, Pawn, AbilitySystemComponent, and Widgets.
- Replicate durable shared state from the class that actually owns it.

## Multiplayer Ownership
- Use GameMode for server-only rules, match flow, and spawn orchestration.
- Use GameState for match-wide replicated state.
- Use PlayerState for durable player-owned replicated state unless a stronger owner is obvious.
- Use Pawn or Character for avatar state that is recreated on respawn.
- Keep ASC ownership explicit and resilient across dedicated server execution and respawn.
- Keep gameplay authority out of widgets.

## Blueprint / C++ Boundary
- Keep core logic and durable state in C++.
- Keep Blueprint-facing APIs stable and explicit.
- When changing Blueprint-facing functions, properties, enums, tags, data assets, or data tables, check for asset drift.
- Use Blueprint hooks for presentation and authored behavior, not core state ownership.

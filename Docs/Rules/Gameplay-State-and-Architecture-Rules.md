# Gameplay State And Architecture Rules

## Architecture Rules
- Managers / Flows / Subsystems orchestrate sequencing, transitions, and assembly only.
- Domain objects own domain rules, state transitions, serialization, and restore logic.
- Prefer interfaces, base classes, delegates, factories, and registries over repeated cast-based branching.
- Prefer adding new types or override points over extending large conditional blocks.
- 예외 처리용 가상함수나 옵션 bool을 새로 만들기 전에, 자료구조/상태머신/도메인 객체로 흡수할 수 있는지 먼저 검토해.

## Blueprint / C++ Boundary
- Keep core logic and durable state in C++.
- Keep Blueprint-facing APIs stable and explicit.
- When changing Blueprint-facing functions, properties, enums, tags, data assets, or data tables, check for asset drift.
- Use Blueprint hooks for presentation and authored behavior, not core state ownership.

## Save / Load Rules
- Runtime state must restore correctly after unload/reload.
- Preserve persistent world changes such as destroyed gimmicks, harvested resources, dead enemies, placements, and mission progress.
- Do not rely on display names as durable identity.
- Be explicit about restore order and dependency order.

# Gameplay State And Architecture Rules

## Ownership Rules
- Managers, flows, and subsystems orchestrate sequencing only.
- Domain objects own domain rules, state transitions, and validation.
- Prefer interfaces, delegates, registries, factories, and polymorphism over repeated cast-based branching.
- Prefer adding a new owner or type over extending a central conditional block.
- Before adding exception bools or one-off hooks, first ask whether the behavior belongs in a data structure, state machine, or domain object.

## Network Ownership Rules
- GameMode owns server-only match rules and admission logic.
- GameState owns replicated match-wide state.
- PlayerState owns durable player-specific replicated state.
- Pawn or Character owns transient avatar state tied to possession or respawn.
- PlayerController owns local input routing, local-only UX decisions, and server RPC initiation for that player.
- Widgets and view models do not own gameplay authority.

## Replication Rules
- Replicate from the real owner, not from a convenient observer.
- Keep write access narrow and explicit.
- Avoid mirrored copies of the same gameplay datum across multiple framework classes.
- If UI needs a projection, derive it from the authoritative owner or a dedicated view model.

## CommonUI And MVVM Boundary
- CommonUI manages screen flow, layered input, and widget lifecycle.
- MVVM view models adapt gameplay state for presentation.
- View models may cache presentation-friendly values, but they must not become gameplay truth.
- Widget events should route intent upward to a gameplay owner, not apply gameplay state directly.

## GAS Boundary
- Keep core ASC ownership and initialization in gameplay classes, not in UI code.
- Be explicit about OwnerActor, AvatarActor, possession timing, and reinitialization on respawn.
- Attribute sets, granted abilities, startup effects, and input binding should have one clear initialization path.
- Server grants authority. Clients present predicted or replicated results through the proper GAS path.

## Blueprint And C++ Boundary
- Keep durable gameplay logic and state in C++.
- Keep Blueprint-facing APIs stable and explicit.
- When changing Blueprint-facing functions, properties, enums, tags, data assets, or data tables, check for asset drift.
- Use Blueprint for authored content and presentation hooks, not hidden ownership transfer.

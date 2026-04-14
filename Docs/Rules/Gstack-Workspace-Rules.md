# gstack Workspace Rules

- Vendored gstack skills live under `.agents/skills/gstack`.
- Prefer `/gstack-browse` for browser-driven verification.
- If gstack skills stop loading, rerun `cd .agents/skills/gstack && ./setup --host codex`.
- Alias skill entrypoints under `.agents/skills/gstack-*` are lightweight indexes; the canonical skill bodies live under `.agents/skills/gstack/<skill>/SKILL.md`.

# OMX Model Capability Table

This table was moved out of `AGENTS.md` so the runtime contract stays compact.

| Role | Model | Reasoning Effort | Use Case |
| --- | --- | --- | --- |
| Frontier (leader) | `gpt-5.4` | high | Primary leader/orchestrator for planning, coordination, and frontier-class reasoning. |
| Spark (explorer/fast) | `gpt-5.3-codex-spark` | low | Fast triage, explore, lightweight synthesis, and low-latency routing. |
| Standard (subagent default) | `gpt-5.4-mini` | high | Default standard-capability model for installable specialists and secondary worker lanes unless a role is explicitly frontier or spark. |
| `explore` | `gpt-5.3-codex-spark` | low | Fast codebase search and file/symbol mapping (fast-lane, fast). |
| `analyst` | `gpt-5.4` | medium | Requirements clarity, acceptance criteria, hidden constraints. |
| `planner` | `gpt-5.4` | medium | Task sequencing, execution plans, and risk flags. |
| `architect` | `gpt-5.4` | high | System design, boundaries, interfaces, and tradeoffs. |
| `debugger` | `gpt-5.4-mini` | high | Root-cause analysis, regression isolation, and failure diagnosis. |
| `executor` | `gpt-5.4` | high | Code implementation, refactoring, and feature work. |
| `team-executor` | `gpt-5.4` | medium | Supervised team execution for conservative delivery lanes. |
| `verifier` | `gpt-5.4-mini` | high | Completion evidence, claim validation, and test adequacy. |
| `style-reviewer` | `gpt-5.3-codex-spark` | low | Formatting, naming, idioms, and lint conventions. |
| `quality-reviewer` | `gpt-5.4-mini` | medium | Logic defects, maintainability, and anti-patterns. |
| `api-reviewer` | `gpt-5.4-mini` | medium | API contracts, versioning, and backward compatibility. |
| `security-reviewer` | `gpt-5.4` | medium | Vulnerabilities, trust boundaries, and authn/authz. |
| `performance-reviewer` | `gpt-5.4-mini` | medium | Hotspots, complexity, and memory/latency optimization. |
| `code-reviewer` | `gpt-5.4` | high | Comprehensive review across all concerns. |
| `dependency-expert` | `gpt-5.4-mini` | high | External SDK/API/package evaluation. |
| `test-engineer` | `gpt-5.4` | medium | Test strategy, coverage, and flaky-test hardening. |
| `quality-strategist` | `gpt-5.4-mini` | medium | Quality strategy, release readiness, and risk assessment. |
| `build-fixer` | `gpt-5.4-mini` | high | Build/toolchain/type-failure resolution. |
| `designer` | `gpt-5.4-mini` | high | UX/UI architecture and interaction design. |
| `writer` | `gpt-5.4-mini` | high | Documentation, migration notes, and user guidance. |
| `qa-tester` | `gpt-5.4-mini` | low | Interactive CLI/service runtime validation. |
| `git-master` | `gpt-5.4-mini` | high | Commit strategy, history hygiene, and rebasing. |
| `code-simplifier` | `gpt-5.4` | high | Simplifies recently modified code without changing behavior. |
| `researcher` | `gpt-5.4-mini` | high | External documentation and reference research. |
| `product-manager` | `gpt-5.4-mini` | medium | Problem framing, personas/JTBD, and PRDs. |
| `ux-researcher` | `gpt-5.4-mini` | medium | Heuristic audits, usability, and accessibility. |
| `information-architect` | `gpt-5.4-mini` | low | Taxonomy, navigation, and findability. |
| `product-analyst` | `gpt-5.4-mini` | low | Product metrics, funnel analysis, and experiments. |
| `critic` | `gpt-5.4` | high | Plan/design critical challenge and review. |
| `vision` | `gpt-5.4` | low | Image, screenshot, and diagram analysis. |

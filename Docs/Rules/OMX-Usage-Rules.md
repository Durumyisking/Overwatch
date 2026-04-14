# OMX Usage Rules

## Default Policy
- OMX workflows are opt-in in this repository.
- Do not activate OMX skills from plain-language intent alone.
- Only explicit `$name` invocations activate OMX workflow skills.

## Direct Work Default
- If the user does not explicitly invoke an OMX skill, execute directly under the normal coding workflow.
- Do not read OMX workflow docs unless an OMX skill is explicitly requested or already active.

## When Explicitly Invoked
- If the user explicitly invokes an OMX skill, follow the corresponding workflow contract in `AGENTS.md`.
- For OMX capability and role details, refer to `Docs/omx-model-capabilities.md` when relevant.

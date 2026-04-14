# Git Workspace Rules

- Only create a worktree when the user explicitly asks for one.
- Otherwise do not run `git worktree add` and do not create separate worktree directories.
- Unless explicitly instructed otherwise, always work in the current checked-out working tree.
- If a new branch is needed, create it in the current working tree only.

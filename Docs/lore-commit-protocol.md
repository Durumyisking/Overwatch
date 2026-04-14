# Lore Commit Protocol

Every commit message should follow the Lore protocol: intent first, narrative context second,
then git-native trailers that preserve decision context for future maintainers.

## Format

```text
<intent line: why the change was made, not what changed>

<body: narrative context — constraints, approach rationale>

Constraint: <external constraint that shaped the decision>
Rejected: <alternative considered> | <reason for rejection>
Confidence: <low|medium|high>
Scope-risk: <narrow|moderate|broad>
Directive: <forward-looking warning for future modifiers>
Tested: <what was verified (unit, integration, manual)>
Not-tested: <known gaps in verification>
```

## Rules

1. The first line explains why, not what.
2. Trailers are optional, but include the ones that add future value.
3. Use `Rejected:` to prevent the same dead-end exploration from repeating.
4. Use `Directive:` for forward-looking warnings or constraints.
5. Use `Constraint:` for forces that are not visible from the diff.
6. Be explicit with `Not-tested:` when verification gaps remain.
7. Keep trailers in git-native `Key: value` format.

## Example

```text
Prevent silent session drops during long-running operations

The auth service returns inconsistent status codes on token
expiry, so the interceptor catches all 4xx responses and
triggers an inline refresh.

Constraint: Auth service does not support token introspection
Constraint: Must not add latency to non-expired-token paths
Rejected: Extend token TTL to 24h | security policy violation
Rejected: Background refresh on timer | race condition with concurrent requests
Confidence: high
Scope-risk: narrow
Directive: Error handling is intentionally broad (all 4xx) — do not narrow without verifying upstream behavior
Tested: Single expired token refresh (unit)
Not-tested: Auth service cold-start > 500ms behavior
```

## Common Trailers

| Trailer | Purpose |
| --- | --- |
| `Constraint:` | External force that shaped the decision |
| `Rejected:` | Alternative that was considered and rejected |
| `Confidence:` | Author confidence (`low`, `medium`, `high`) |
| `Scope-risk:` | Expected blast radius (`narrow`, `moderate`, `broad`) |
| `Reversibility:` | Ease of rollback (`clean`, `messy`, `irreversible`) |
| `Directive:` | Forward-looking warning for future modifiers |
| `Tested:` | Verification that was completed |
| `Not-tested:` | Honest verification gaps |
| `Related:` | Related commit, issue, or decision reference |

Teams may add domain-specific trailers when useful.

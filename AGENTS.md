## Instruction precedence and scope

- Follow the direct task request first, then more-specific nested repository guidance, repository configuration, and this file.
- Before editing, inspect nearby code plus relevant configuration (`pyproject.toml`, dependency files, and test configuration).
- Preserve unrelated user changes. Keep changes limited to the request, accepted specification, and necessary verification.
- Do not add, remove, or upgrade dependencies; alter build configuration; or perform unrelated refactors unless required by the task or explicitly requested.

## Specification-first delivery

Treat a clear user request, issue, or approved design document as the specification.

For material changes—new user-visible behavior, public APIs, data-model changes, security-sensitive work, multi-module refactors, or irreversible operations—ensure the specification states:

- intended behavior and non-goals;
- affected interfaces, inputs, and outputs;
- important edge cases and failure behavior;
- acceptance criteria that can be verified.

If material ambiguity would affect behavior, scope, security, or architecture, do not guess silently. State the relevant assumption, ask focused questions, or propose a concise specification for approval.

For small, low-risk, clearly bounded changes, infer reasonable details from existing code, tests, and configuration.

## Completion

Before declaring completion:

- Confirm the implementation meets the request or accepted specification.
- Confirm no new unfinished scaffolding or invented APIs were added.
- Confirm relevant tests and validation checks passed, or clearly state what could not be verified.
- Report the implemented behavior, files changed, validation performed, and any remaining limitation.

<!-- codev:start -->
## CoDev human-AI delivery

Read `.codev/for-ai/ai-agent-guidelines.md` before planning or implementing product
work. Route requests internally through the installed skills and describe the
current human-facing step as `Understand`, `Build`, `Review`, or `Ship`.

Use the lightest safe path. Inspect repository facts before prescribing code,
keep changes bounded and reviewable, run proportionate validation, and stop for
material decisions instead of inventing them. Humans retain authority for
acceptance, merge, deployment, migration, publication, and rollout expansion.
<!-- codev:end -->

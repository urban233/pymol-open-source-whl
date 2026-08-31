# Skill Card: audit-google-python-style

**Description:** Audits Python code against the Google Python Style Guide using this repository's Ruff and `pymake` toolchain plus supplemental analysis, then proposes a grouped remediation plan for explicit human approval before modifying any approved source file.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Invoke only when a developer explicitly requests this audit or invokes `$audit-google-python-style`. Not for ordinary code reviews, pull-request reviews, linting, or implementation tasks -- those have their own skills (`review-change`, `pr-review`, `build-change`).

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** Ruff and this repository's own `pymake` build tool; git; an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could be reached for as a general-purpose code reviewer, which it is not -- mitigated by the frontmatter's explicit "do not use for ordinary code reviews" scope and by requiring explicit invocation rather than automatic triggering. Its planning phase is read-only by design (no source file is modified) until the human explicitly approves the proposed plan, limiting the blast radius of a wrong or incomplete audit.

**References:** This skill's own `SKILL.md`; `docs/features/skill-eval/how-to-write-a-task.md` (the `audit-google-python-style-demo` eval task built against this skill); `docs/adr/0028-skill-packages-carry-their-own-eval-trace.md`.

**Skill Output:** A short, grouped remediation plan for human approval; once approved, edits to the specific approved source files.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

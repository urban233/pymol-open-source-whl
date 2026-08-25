# Skill Card: review-change

**Description:** Independently reviews a pull request, commit, patch, or working-tree diff for correctness, regressions, security, test quality, maintainability, scope, and conformance to an accepted brief or design.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when a developer requests code review, a second AI pass, pre-merge assurance, or an evidence-based quality gate. Its natural home is a diff with no task and no open pull request -- once a CoDev-built task has an open PR, the outer loop's specialist review covers this same ground automatically.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None beyond git and an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could blur into implementation if it modified code while reviewing -- mitigated by reviewing only the exact supplied snapshot and not modifying code unless explicitly asked afterward. This is the ground truth the `seeded-defect-*` eval corpus measures (see `docs/adr/0001-work-lifecycle-invariant.md` for why that corpus exists).

**References:** This skill's own `SKILL.md`; `docs/adr/0001-work-lifecycle-invariant.md`; `docs/adr/0005-review-family-consolidation.md`.

**Skill Output:** An independent review: findings by dimension, and a verdict.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

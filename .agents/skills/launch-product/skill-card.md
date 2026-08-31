# Skill Card: launch-product

**Description:** Prepares and guides a safe release of a completed feature or product through readiness review, migration, feature flags, internal testing, staged rollout, observability, rollback, and post-launch learning.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when code is approaching deployment, a team needs a launch checklist or rollout plan, or production evidence must determine expansion.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None beyond git and an authenticated OpenCode (or equivalent agent) invocation; references whatever feature-flag or observability tooling the target project already has.

**Known Risks and Mitigations:** The highest-stakes skill in this bundle by nature (it governs real user exposure) -- mitigated by an explicit, load-bearing constraint: "Do not deploy, publish, or enable users without explicit human authorization." This skill assembles readiness evidence and proposes rollout stages; it never expands exposure on its own authority.

**References:** This skill's own `SKILL.md`.

**Skill Output:** A launch readiness assessment, rollout-stage plan, and rollback criteria.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

# Skill Card: design-solution

**Description:** Creates or revises a practical technical design for a significant feature, product, migration, or cross-component change: architecture, ownership, contracts, data flow, trade-offs, risk controls, test strategy, and rollout design.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when engineers need a shared technical decision recorded before implementation. Skip this skill for local, low-risk changes with an obvious implementation.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None beyond git and an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could be invoked for trivial changes that don't need this much process -- mitigated by the explicit scope boundary ("Skip this skill for local low-risk changes with an obvious implementation"). Durable decisions this skill produces are recorded as append-only ADRs, so a later reversal is a new ADR, never a silent edit to the record of what was decided.

**References:** This skill's own `SKILL.md`; `assets/adr.template.md`; `docs/adr/0025-formalize-adr-practice.md`.

**Skill Output:** A design document, and an ADR for each durable, cross-cutting decision it makes.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

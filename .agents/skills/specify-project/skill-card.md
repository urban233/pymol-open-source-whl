# Skill Card: specify-project

**Description:** Interviews a developer one question at a time to create or revise a canonical `SPECIFICATION.md` combining an accepted product frame with a high-level technical blueprint.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use for a new greenfield product, a whole-product redesign, or an explicit request for a comprehensive project specification. Not for a bounded feature, implementation plan, roadmap, task breakdown, or code change.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None beyond git and an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could be reached for on a routine, bounded change where it is far too heavyweight -- mitigated by the explicit scope boundary and its own stop conditions (e.g. stopping when product answers conflict or a material decision has no accountable human).

**References:** This skill's own `SKILL.md`.

**Skill Output:** A canonical, accepted `SPECIFICATION.md`.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

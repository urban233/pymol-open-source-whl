# Skill Card: define-product

**Description:** Turns a software idea, product proposal, or feature request into a clear product or feature brief and selects the lightest safe workflow.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when a developer needs help clarifying users, outcomes, scope, success measures, constraints, or assumptions. Does not design the technical solution or create implementation tasks -- those belong to `design-solution` and `plan-delivery`.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None beyond git and an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could drift into technical design decisions that belong elsewhere -- mitigated by the explicit scope boundary ("Do not design the technical solution or create implementation tasks").

**References:** This skill's own `SKILL.md`; `design-solution`; `plan-delivery`.

**Skill Output:** A product or feature brief, and a workflow-sizing recommendation.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

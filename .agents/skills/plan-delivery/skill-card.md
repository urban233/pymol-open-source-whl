# Skill Card: plan-delivery

**Description:** Turns an accepted product or feature brief and any required design into a lightweight, team-profile-aware multi-developer delivery plan: milestones, capability lanes, owners, independent reviewers, dependencies, and WIP limits.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when a team needs to coordinate multiple developers on related work. Does not create a separate architecture or capacity bureaucracy -- that scope stays with `design-solution` and the plan itself stays lightweight.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None beyond git and an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could produce process overhead disproportionate to the work -- mitigated by the explicit non-goal ("Do not create a separate architecture or capacity bureaucracy") and its focus on outcome-based milestones over prediction.

**References:** This skill's own `SKILL.md`; `define-product`; `design-solution`.

**Skill Output:** A delivery plan: milestones, lanes or ready tasks, owners, reviewers, dependencies, and risks.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

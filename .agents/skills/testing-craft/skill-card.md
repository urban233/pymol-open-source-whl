# Skill Card: testing-craft

**Description:** Provides Google-derived testing strategy, test-writing craft, and test-suite health reference, distilled from *Software Engineering at Google* (the Flamingo book), the Google Testing Blog, and Google's published testing-infrastructure research.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Read automatically by `specify-project` and `design-solution` before deciding test scope, and by `build-change` before adding or updating tests. Invoke directly to design a test strategy, audit an existing suite's health, or triage a flaky or brittle test.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None.

**Known Risks and Mitigations:** As a reference other skills consult rather than an autonomous actor, its main risk is guidance being applied out of context -- mitigated by being scoped to informing another skill's own judgment, never substituting for it.

**References:** This skill's own `SKILL.md`.

**Skill Output:** A test strategy, a well-crafted test, or a test-suite health audit.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

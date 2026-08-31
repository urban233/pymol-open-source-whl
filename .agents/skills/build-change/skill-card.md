# Skill Card: build-change

**Description:** Pairs with a developer to investigate, plan, implement, test, and prepare one bounded code change, bug fix, refactor, or delivery-plan task, with frequent checkpoints and human control.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use for hands-on AI-assisted coding on one bounded change, not a long autonomous implementation loop. Grounds every plan in the current repository and keeps changes small and reviewable.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** git; an authenticated OpenCode (or equivalent agent) invocation; whatever build/test tooling the specific change already depends on (e.g. the project's own test runner).

**Known Risks and Mitigations:** Could grow into a large, hard-to-review change if left unchecked -- mitigated by frequent human checkpoints and the explicit expectation of small, reviewable changes. Stops and asks for one precise decision when required behavior conflicts, a material design choice is missing, permissions or a dependency are unavailable, or safe validation cannot be produced (see its own Stop conditions).

**References:** This skill's own `SKILL.md`; `docs/adr/0001-work-lifecycle-invariant.md`; the `testing-craft` skill, read before adding or updating tests.

**Skill Output:** An implemented, tested code change prepared for independent review (and, once a task exists, a pull request).

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

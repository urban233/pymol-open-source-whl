# Skill Card: critique-review

**Description:** Turns an existing code-review finding, presubmit failure, or lint result into a precise, reviewable suggested diff, while preserving independent read-only review.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use only after a review finding or presubmit failure needs a concrete proposed edit. Requires an explicit developer or `build-change` handoff before any file is modified -- this skill itself never applies a suggestion directly.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** git; an authenticated OpenCode (or equivalent agent) invocation.

**Known Risks and Mitigations:** Could blur the line between independent review and implementation if it applied its own suggested fixes -- mitigated by design: it produces suggested diffs and an explicit handoff, and does not modify files itself.

**References:** This skill's own `SKILL.md`; the `review-change` skill.

**Skill Output:** A precise, reviewable suggested diff, with an explicit handoff for someone else to apply it.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

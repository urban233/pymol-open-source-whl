# Skill Card: pr-review

**Description:** Reviews an existing GitHub Pull Request as an exact merge candidate and prepares evidence-based inline review comments for GitHub.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use only when a developer asks to review a GitHub PR before merge, inspect its description and checks, or publish a pending PR review. Not for general code review, commit review, branch review, or working-tree review -- those belong to `review-change`.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** An authenticated `gh` CLI (falls back to it automatically when no `GITHUB_TOKEN`/`GH_TOKEN` is set), or `GITHUB_TOKEN`/`GH_TOKEN` directly for publishing.

**Known Risks and Mitigations:** Could be mistaken for something that can also merge or apply changes -- mitigated by being read-only with respect to code: it reviews the exact PR head and can prepare or publish a pending review, but does not apply code, approve, merge, or release (see `AGENTS.md`).

**References:** This skill's own `SKILL.md`; `AGENTS.md`'s Human-AI Development Policy.

**Skill Output:** A prepared (and optionally published) GitHub PR review with anchored inline comments.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

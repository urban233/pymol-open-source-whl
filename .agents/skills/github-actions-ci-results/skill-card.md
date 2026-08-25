# Skill Card: github-actions-ci-results

**Description:** Reads, inspects, and summarizes GitHub Actions workflow runs: run status, jobs, steps, annotations, and relevant logs.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when a developer asks what happened in a CI run, why CI failed, which job or step failed, whether a run is still in progress, or wants a concise CI result for a pull request or incident update.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** An available authenticated GitHub connector, or the authenticated `gh` CLI (checked via `gh auth status` without exposing tokens).

**Known Risks and Mitigations:** Could be mistaken for a tool that can also re-run or cancel workflows -- mitigated by being explicitly read-only: it "reads GitHub Actions results without changing repository or workflow state".

**References:** This skill's own `SKILL.md`.

**Skill Output:** A concise summary of a CI run's outcome, with the smallest useful log excerpts needed to explain it.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

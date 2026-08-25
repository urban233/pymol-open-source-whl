# Skill Card: design-skill-eval

**Description:** Guides a developer through designing and scaffolding one new eval task for an installed skill's performance-evaluation corpus: a falsifiable ground truth, a prompt that never names the skill, a deterministic verifier, and a judge rubric.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Use when adding eval coverage for an existing skill, or bootstrapping the first task for a skill that has none. Not for running an existing benchmark, building or editing the skill under test itself (that is ordinary `build-change` work), or general code review.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** git; an authenticated OpenCode (or equivalent agent) invocation; the `codev` CLI.

**Known Risks and Mitigations:** Could be reached for to modify the skill under test itself, which is out of scope -- mitigated by an explicit redirect to `build-change` for that case. A poorly designed task (rubric-only where a seeded defect was possible, or a task that doesn't discriminate between conditions) can silently produce a meaningless result -- mitigated by this skill's own design checklist and its required step 11 (prove the task actually discriminates before treating it as done).

**References:** This skill's own `SKILL.md` and `references/eval-design-checklist.md`; `docs/features/skill-eval/how-to-write-a-task.md`; `docs/adr/0028-skill-packages-carry-their-own-eval-trace.md`.

**Skill Output:** One committed eval task under `.codev/eval/tasks/<name>/` (`task.json`, `prompt.md`, `rubric.md`, and either `checks.json` or `verifier.json`).

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

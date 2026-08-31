# Skill Card: [skill-name]

> **How to use this file.** Copy it to `skill-card.md` inside the skill's own
> directory (`.agents/skills/<name>/skill-card.md`), alongside its `SKILL.md`.
> Fill in every section with real, verifiable facts about this specific skill
> -- never invent a license, a dependency, or a risk that isn't actually true.
> A skill card exists so a reviewer can understand a skill's purpose, owner,
> output, and risks without opening its source first. This mirrors NVIDIA
> SkillEvaluator's Recommended Artifact Set skill card
> (docs.nvidia.com/skills/agent-skill-trust-pipeline); CoDev does not gate
> anything on this file's presence, so an incomplete or missing card is not a
> hard failure -- but an accurate one is expected for every skill you ship.

**Description:** [One or two sentences: what this skill does. Usually a
trimmed version of `SKILL.md`'s own frontmatter `description`.]

**Owner:** [Who is responsible for this skill's behavior and upkeep -- a
person, a team, or an organization name.]

**License / Terms of Use:** [The license covering this skill's own content --
usually the repository's own license, unless the skill bundles or depends on
something under different terms. Match `SKILL.md`'s frontmatter `license`
field.]

**Use Case:** [When a developer should reach for this skill, and what it is
explicitly not for -- drawn from `SKILL.md`'s own description and any stated
non-goals, not invented.]

**Deployment Geography for Use:** [Where and how this skill actually runs. For
a skill with no hosted service or region-specific dependency: "Not
applicable -- runs locally inside the installing developer's own repository
and toolchain; not hosted or operated as a service anywhere."]

**Requirements / Dependencies:** [Real external tools, credentials, or
services this skill actually invokes -- an authenticated `gh` CLI, a specific
linter, network access, a GPU, a third-party model or API. State "None beyond
git and an authenticated OpenCode (or equivalent agent) invocation" if that
really is all it needs -- do not list a dependency the skill doesn't use.]

**Known Risks and Mitigations:** [What could go wrong if this skill is misused
or over-trusted, and what already prevents or limits that -- drawn from the
skill's own stated non-goals, scope boundaries, and stop conditions. Don't
invent a risk beyond what the skill's own documented behavior already
implies.]

**References:** [Links to this skill's own `SKILL.md`, and any ADR, design
doc, or external specification (a paper, a model license, a service's terms)
that governs its behavior or a dependency it relies on.]

**Skill Output:** [What this skill actually produces when it runs -- a
document, a pull request, review comments, a plan, structured findings, etc.]

**Skill Version:** [Tie this to the CoDev release this skill ships with,
since skills are not independently versioned: "Versioned with the installed
CoDev release."]

**Ethical Considerations:** [Anything beyond ordinary safe operation worth
naming explicitly -- for a skill with no special ethical surface: "Human
retains authority for acceptance, merge, deployment, and publication (see
`AGENTS.md`'s Human-AI Development Policy); this skill does not act
autonomously beyond that boundary." Add anything genuinely specific to this
skill (a third-party model's own stated use restrictions, a deployment
geography restriction, a data-handling concern) rather than leaving the
generic line if something more specific is true.]

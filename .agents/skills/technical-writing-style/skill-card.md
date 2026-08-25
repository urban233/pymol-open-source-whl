# Skill Card: technical-writing-style

**Description:** Provides the Google-derived technical writing style reference and revision method for any reviewer-facing technical document: a specification, brief, design, delivery plan, or launch plan.

**Owner:** CoDev maintainers

**License / Terms of Use:** BSD-3-Clause (this repository's own `LICENSE`; matches this skill's `SKILL.md` frontmatter `license` field).

**Use Case:** Read automatically by `specify-project`, `define-product`, `design-solution`, `plan-delivery`, and `launch-product` before they draft or revise prose. Invoke directly to audit or revise the writing quality of an existing document without changing its technical meaning.

**Deployment Geography for Use:** Not applicable -- runs locally inside the installing developer's own repository and toolchain; CoDev does not host, deploy, or operate this skill as a service.

**Requirements / Dependencies:** None.

**Known Risks and Mitigations:** Could change a document's technical meaning while revising its prose -- mitigated by the explicit constraint that a revision must not change technical meaning.

**References:** This skill's own `SKILL.md` and `references/writing-style.md`.

**Skill Output:** A revised document, or a written style critique of an existing one.

**Skill Version:** Versioned with the installed CoDev release (currently 0.3.0).

**Ethical Considerations:** Human retains authority for acceptance, merge, deployment, and publication (see `AGENTS.md`'s Human-AI Development Policy); this skill does not act autonomously beyond that boundary.

---
name: technical-writing-style
description: Provides the Google-derived technical writing style reference and revision method for any reviewer-facing technical document -- a specification, brief, design, delivery plan, or launch plan. The planning skills (`specify-project`, `define-product`, `design-solution`, `plan-delivery`, `launch-product`) read this skill's reference before drafting or revising prose. Invoke directly to audit or revise the writing quality of an existing document without changing its technical meaning.
license: BSD-3-Clause
---

# Technical Writing Style

Reference and revision method for making a technical document clear,
correctly scoped, and readable by its actual audience -- without changing
what it means. This skill has no template of its own; it makes the prose
in another skill's artifact readable.

## Invocation boundary

Use this skill in two situations:

- **As a prerequisite.** `specify-project`, `define-product`,
  `design-solution`, `plan-delivery`, and `launch-product` each instruct,
  in their own `SKILL.md`, reading
  `.agents/skills/technical-writing-style/references/writing-style.md`
  before drafting or revising a document's prose. That is a read, not a
  full invocation of this skill -- continue in the calling skill's own
  workflow and template.
- **Directly**, when the developer asks to improve, simplify, or audit the
  writing quality of an existing technical document, or invokes
  `$technical-writing-style`.

## Apply the reference

Read `references/writing-style.md` completely before touching any prose.
It covers reader-first framing, document opening and structure, headings,
paragraphs, lists and tables, sentence-level language, terminology,
global/inclusive writing, formatting, and diagrams, plus a self-check.

## Run the structural check

Before or alongside the manual pass, run `scripts/check_structure.py
<files...>` (or `--root <dir>` to check every `*.md` file directly in one
directory) against the document or document set. It catches what a
read-through is prone to miss: a broken link or anchor, a heading with no
content before its first subheading, a table column that repeats nearly
the same value in every row, a table wide enough that its cells are
carrying prose instead of comparable values, and a paragraph that closely
matches one in a different file.

Treat `violation`-level findings as mechanically certain -- fix them.
Treat `review`-level findings, mainly the cross-file duplicate check, as
needing judgment before acting: a short, deliberately worded pointer
sentence repeated across sibling documents (`See the parent design's
Goals and non-goals...`) is the correct de-duplication pattern, not a
defect, even though its wording is near-identical to a sibling's. Check
what each match is actually doing -- restating a fact, or pointing at
one -- before touching it. The script cannot make that distinction; only
its `violation`-level findings can be trusted without a second look.

## When invoked directly on an existing document

1. Read the whole document. Identify its actual audience, scope, and the
   decision or action it exists to support -- infer this from the
   document's own metadata (owner, reviewers, linked brief) and content
   when the developer does not state it.
2. Preserve every technical fact, requirement, number, warning, decision,
   and open question exactly. Never invent a feature, prerequisite,
   rationale, or measurement, and never soften a stated uncertainty into a
   settled fact or a settled fact into a hedge.
3. Revise structure and prose against `references/writing-style.md`: turn
   enumerable run-on sentences into lists, define jargon a named reader
   might not know, split paragraphs and sections that carry more than one
   topic, and rewrite passive or nominalized sentences where an active one
   is clearer.
4. If the document covers more than one independently reviewable
   subsystem or concern, say so and point to the owning skill's own
   splitting rule (`design-solution`'s parent/child rule is the current
   example) rather than silently restructuring the document yourself --
   document architecture belongs to the owning skill, not this one.
5. Report what changed at a level the developer can verify quickly: which
   sections were restructured, which terms were defined, and any fact you
   were unsure how to preserve -- ask rather than guess.

## Handoff

This skill does not decide document architecture, ownership, or
acceptance -- it only makes existing or in-progress content clear. Return
control to the calling planning skill, or to the developer, once the
self-check in `references/writing-style.md` passes.

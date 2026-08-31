---
name: design-solution
description: Create or revise a practical technical design for a significant feature, product, migration, or cross-component change. Use when engineers need architecture, component ownership, APIs or contracts, data flow, trade-offs, risk controls, test strategy, or rollout design before implementation. Skip this skill for local low-risk changes with an obvious implementation.
license: BSD-3-Clause
---

# Design Solution

Help the developer make the few technical decisions that must be shared before
parallel implementation. Use `assets/design.template.md` for the design, or
for each child design once split (see step 2); use `assets/adr.template.md`
only for a durable cross-cutting decision that must outlive the design
document -- an Architecture Decision Record (ADR). Read
`.agents/skills/technical-writing-style/references/writing-style.md`
completely before drafting or revising a design document's prose.

## 1. Establish context

Read the accepted brief, relevant repository instructions, current architecture,
code, tests, and prior decisions. Confirm the design still solves the stated
outcome. Return to `define-product` if the outcome or scope is the real problem.

State which decisions the design must settle and which details can safely remain
with implementing engineers.

## 2. Decide the document's shape

Decide whether this is one design or a parent with independent children
before investigating or drafting. Split when the work covers more than one
subsystem that each has its own components, contracts, alternatives, and
risk profile, and that a reviewer could accept on its own -- not merely
because the topic is broad or the likely document will be long. Two
signals, not hard limits: the Proposed design section would need more than
one independent Components/APIs/Alternatives block, or the reviewers
needed for different parts of the document do not overlap and each would
have to read past substantial unrelated subsystem detail to reach their
own part.

When splitting:

- Keep entangled cross-cutting material in the **parent**: Summary, Goals
  and non-goals, the shared component map, cross-subsystem contracts, and
  any open question that blocks more than one child. Save it at
  `docs/codev/design/<slug>/design.md` as usual.
- Give each independent subsystem its own **child** design at
  `docs/codev/design/<slug>/<child-slug>.md`, using this same template and
  skill, scoped to that subsystem's own components, alternatives, risk,
  and test strategy.
- Link every child from the parent's Proposed design section with a
  one-line scope. Set each child's `Parent design` header field to link
  back. Never duplicate a fact between parent and child -- state it once
  and link to it.
- A child can be drafted, reviewed, and marked `Accepted` on its own
  schedule. The parent depends on a child only for the specific decisions
  it names as blocking, not for that child's unrelated detail.
- Do not split a design that covers one coherent subsystem merely because
  it is long. Splitting adds a document and a cross-reference to maintain,
  and only pays for itself when it lets a reviewer skip content that is
  not theirs.

Revisit this decision in step 7 if drafting reveals the scope is more
entangled, or more separable, than it first appeared.

## 3. Investigate before proposing

Locate existing components, ownership, extension points, schemas, APIs, failure
conventions, deployment model, and comparable implementations. Distinguish
verified repository facts from assumptions.

For material choices, present the recommended option, meaningful alternatives,
and trade-offs. Ask the human only when alternatives change product behavior,
an interface, persistent data, risk, cost, or ownership.

## 4. Design stable boundaries

Describe components in ordinary language. For every cross-component API or
contract, define:

- owner, callers, and purpose;
- request/event/data shape or authoritative reference;
- guarantees and caller obligations;
- validation, errors, timeouts, and retries;
- compatibility and migration expectations; and
- a contract-level test or fixture when parallel work depends on it.

Do not prescribe classes, private methods, file layouts, or algorithms unless
they are genuinely architectural.

## 5. Design quality and delivery

Cover proportionate concerns:

- security, privacy, permissions, abuse, and data retention;
- reliability, concurrency, observability, capacity, and cost;
- accessibility and internationalization;
- unit, contract, integration, end-to-end, performance, and failure testing;
- migration, feature flag, rollout, rollback, and cleanup; and
- unresolved risks with an owner and evidence-producing next step.

Read `.agents/skills/testing-craft/references/test-strategy.md` before
deciding the size/scope mix and which larger tests earn their cost for the
bullet above, and run its self-check
(`references/test-strategy.md#5-self-check-before-accepting-a-strategy` in
that skill) against the design's Test strategy section.

Prefer a thin end-to-end path that can be tested early.

## 6. Write for the reviewers, not for yourself

The design's readers are the named reviewers and the accountable owner making
an accept/reject decision -- not another agent operating under a token
budget. Do not carry this repository's own dense, agent-facing house style
into a document a human must read and approve.

Read `.agents/skills/technical-writing-style/references/writing-style.md`
completely before drafting or revising any prose, and apply it -- do not
treat it as optional background. Run its self-check
(`references/writing-style.md#11-self-check-before-saving` in that skill)
against your draft before moving to step 7.

## 7. Review and accept

Before saving, confirm step 2's shape decision still holds: if drafting
pulled in a second independent subsystem, or the whole document turned out
to be one coherent scope after all, split or merge now rather than saving
something the shape rule would have rejected.

When step 2 produced a parent and children, also run this audit before
saving. A single-document read-through does not catch these -- splitting
creates failure modes that only exist across documents. Start with
`python3 .agents/skills/technical-writing-style/scripts/check_structure.py
<parent.md> <child1.md> ...` -- it mechanically catches a broken
cross-document link or anchor, a heading with no content before its first
subheading, and a redundant table column in every file, not only the one
where the defect was first noticed. Fix every `violation`-level finding
outright, and use judgment on `review`-level findings (see that skill's
own guidance on its cross-file duplicate check before acting on one).
Then still work through the rest of this list by hand -- the script has no
way to know whether two similar sentences assert the same fact or merely
share a template pattern, which of two overlapping claims is the one to
keep, or whether a bullet's meaning survived being split across files:

- Diff every child's opening paragraph against the parent's Summary and
  Goals. If the same claim appears in both, delete one side and link to
  the other instead of restating it -- including a claim the child
  attributes back to the parent in passing; attribution is not an
  exemption from the no-duplication rule.
- Re-check every acronym or term's first use independently in each
  resulting file. A definition that now lives only in a sibling file does
  not satisfy `references/writing-style.md`'s first-use rule for this
  file's own reviewer.
- Re-check heading-to-subheading spacing in every resulting file, not only
  the one it was first fixed in -- the same structural fix does not
  propagate to siblings on its own.
- Re-run the redundant-column check against every table in every
  resulting file, including a table that looks nothing like the one that
  originally prompted the check -- the same shape usually hides the same
  defect.
- Count goals, non-goals, components, alternatives, and open-question
  bullets before and after the split at the clause level, not the bullet
  level. A clause dropped from inside a surviving bullet is invisible to
  a bullet count, and a compound bullet split across two children is
  exactly where this happens.
- Apply the sentence-density rule to prose written during the split itself
  -- new summaries, new transition sentences -- with the same rigor as
  prose carried over from the original. New prose is not exempt because
  it is new.

Save product designs under `docs/codev/design/` or feature-local designs under
`docs/codev/features/<slug>/design.md`, following repository conventions. Name
an owner and required domain reviewers. Keep open questions visible.

Mark the design `Accepted` only after material decisions are resolved and the
human confirms it is safe to plan against. For a child design, this means
only that child's own decisions; the parent stays `Draft` until every child
it names as blocking is `Accepted`. Git history records revisions.
Implementation discoveries may update the design; explain affected work rather
than invalidating unrelated plans automatically.

Write an ADR (`assets/adr.template.md`) only for a decision that must outlive
this design document -- a choice other future designs will need to find and
respect, not a detail local to this one. Save it at `docs/adr/NNNN-slug.md`,
`NNNN` the next four-digit, zero-padded sequence number after the highest one
already in `docs/adr/` (start at `0001` if the directory doesn't exist yet).
An ADR is append-only once `Accepted`: never edit a past ADR's `Context` or
`Decision` to reflect new information -- write a new ADR and mark the old one
`Superseded by ADR-NNNN` instead. Link it from the design document; do not
duplicate its content there.

## Handoff

Send the accepted brief, design, API/contract references, open risks, and next
demonstrable outcome to `plan-delivery`. Do not assign people or generate an
exhaustive task list.

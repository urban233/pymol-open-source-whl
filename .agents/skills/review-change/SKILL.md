---
name: review-change
description: Independently review a pull request, commit, patch, or working-tree diff for correctness, regressions, security, test quality, maintainability, scope, and conformance to an accepted brief or design. Use when a developer requests code review, a second AI pass, pre-merge assurance, or an evidence-based quality gate. Review only the exact supplied snapshot and do not modify code unless explicitly asked afterward. Its natural home is a diff with no task and no open pull request — once a CoDev-built task has an open PR, the outer loop's specialist review covers this same ground automatically.
license: BSD-3-Clause
---

# Review Change

Act as an independent reviewer, not a second implementer. Review the exact
base-to-head snapshot and state the snapshot when possible.

## Preconditions

Read the issue or task, acceptance criteria, relevant brief/design/API,
repository instructions, complete diff, and validation evidence. Inspect enough
surrounding code to understand behavior. If the target or evidence is ambiguous,
identify the limitation instead of guessing.

## Review order

Prioritize:

1. incorrect or missing required behavior;
2. security, privacy, permission, data-loss, and compatibility risk;
3. concurrency and race-condition risk — shared state, lock ordering, async
   correctness — as its own concern, not folded into the item above;
4. error handling and material edge cases;
5. test quality, missing tests, and weakened or misleading tests;
6. architecture/API conformance and unnecessary scope;
7. maintainability, clarity, documentation, and repository conventions; and
8. rollout, monitoring, migration, and rollback concerns.

Passing checks are evidence, not proof. Rerun proportionate checks when useful
and authorized. Prefer a few representative integration tests at important
boundaries over exhaustive unit-test enumeration. Coverage percentages are
diagnostic only, never a required quality gate. Do not block on theoretical,
rare, low-impact edge cases unless they create a credible correctness, safety,
data-integrity, compatibility, or regression risk. Do not invent requirements
or block on personal style.

## Findings

Lead with actionable findings, ranked most-important-first. Mark each finding
`blocking` only if it must be fixed before this change can be `READY FOR
HUMAN APPROVAL`; mark everything else non-blocking. This is a binary, not a
graded scale — do not disguise a preference as a blocker, and do not soften a
genuine blocker to avoid conflict.

For each finding give the location, observed evidence, impact, and a precise
testable correction. Keep line ranges tight. If no actionable finding exists,
say so and list residual risks or validation gaps.

## Coverage

Record a `passed`/evidence verdict for every dimension listed under Review
order, every round, even when the verdict is "not applicable to this
change." An omitted dimension is not an implicit pass — silence must never be
mistaken for coverage.

End with one recommendation: `READY FOR HUMAN APPROVAL`, `CHANGES REQUIRED`, or
`BLOCKED BY MISSING EVIDENCE`. Reviewer readiness never authorizes merge or
release. A `READY FOR HUMAN APPROVAL` recommendation requires a complete,
passing coverage record — an incomplete one is not a valid basis for it.

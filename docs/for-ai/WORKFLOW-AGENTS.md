# AI Collaboration Reference

Use this reference when helping a developer move from a software idea to a safe
production release. Operate as an interactive engineering partner, not an
unattended implementation service.

## Present one simple workflow

Route skills internally. Do not make the developer learn or select them. Name
the current human-facing step in plain language:

1. **Understand:** settle the outcome and any material design or coordination.
2. **Build:** implement and validate one bounded change.
3. **Review:** independently inspect an exact change snapshot.
4. **Ship:** authorize and observe controlled production exposure.

Design and delivery planning are conditional work inside `Understand`, not
additional stages every change must traverse.

## Select the path

- **Quick change:** use `build-change`, then `review-change` when risk warrants.
- **Clean Code pass:** use `clean-code-review` when the developer requests a
  catalog-driven smell, maintainability, Clean Code, GoF, or Python-specific
  review; it complements the broader `review-change` pass.
- **GitHub Pull Request:** use `pr-review` only for an existing GitHub PR that
  needs a merge-candidate review or validated inline comments. It is not a
  general branch, commit, or working-tree review.
- **Suggested fix:** use `critique-review` after a concrete review or presubmit
  finding; it drafts a precise diff without modifying files. Require an
  explicit handoff to `build-change` or the developer before applying it, then
  obtain a fresh `review-change` pass.
- **Feature:** use `define-product`; add `design-solution` for material technical
  decisions; use `plan-delivery` for multi-person work; repeat build and review.
- **Guided greenfield/whole product:** use `specify-project` for a one-question-
  at-a-time interview and one canonical `SPECIFICATION.md`, then continue with
  `plan-delivery`.
- **Product/high risk, modular:** use `define-product`, `design-solution`, and all
  later lifecycle skills through `launch-product`.

Risk overrides size. Permission, security, privacy, public API, persistent-data,
billing, compliance, destructive, or difficult-to-reverse changes require a
design and independent review.

## Interaction contract

1. Explain the current step and its value in plain language.
2. Read supplied material and inspect discoverable repository facts before
   asking the developer.
3. Recommend a path or decision; do not present an unfiltered option catalogue.
4. Ask only for decisions that change outcome, scope, architecture, API/data,
   risk, ownership, priority, or commitment.
5. Keep progress visible at meaningful boundaries; never disappear into an
   unattended retry loop.
6. Preserve human control over acceptance, merge, release, migration,
   publication, and rollout expansion.

Before editing in `Build`, present an inline focus card containing the change,
observable success, non-goals, allowed scope, validation, stop conditions, and
work style. Default to interactive pairing. Use bounded delegation only for
isolated, well-specified, testable, reversible work that will be independently
reviewed. Surface a required scope expansion before acting on it.

## Three-agent Build execution

When the platform provides repository-local subagents, keep the human in one
primary orchestrator conversation and automate task-local handoffs:

1. The **orchestrator** reads authority and repository evidence, confirms
   readiness, presents the focus card, and creates any required implementation
   plan. It does not edit product code.
2. After human approval to delegate, the **builder** executes only that accepted
   plan, validates the exact change, and returns an evidence receipt with base
   and head snapshots. It cannot invoke agents or authorize source-control or
   release actions.
3. The orchestrator invokes a fresh, read-only **reviewer** with the exact diff,
   work item, plan, authority, and evidence. The reviewer never fixes its own
   findings.
4. The orchestrator routes actionable findings back to the builder and repeats
   independent review. Stop after two correction attempts with the same root
   cause or whenever a material decision, scope expansion, collision, or
   validation gap requires the human.

Pass durable task facts and observable evidence between agents, not private
reasoning or broad chat transcripts. The human retains plan/delegation, merge,
and release authority. If the platform lacks subagents, one interactive builder
may perform implementation, but review still needs a fresh context and human
approval.

## Artifact authority

- Combined specification: product frame and high-level technical blueprint when
  `specify-project` is deliberately selected. It replaces, rather than copies,
  the corresponding brief and design.
- Brief: why, users, outcome, success, scope, non-goals, constraints.
- Design/API: architecture, ownership, contracts, trade-offs, risk controls.
- Project tracker/delivery plan: milestones, work items, assignments,
  dependencies, status.
- Implementation plan: repository-grounded approach for one bounded work item.
- Code/tests: implemented behavior and executable evidence.
- Launch plan/observability: release decision, exposure, health, learning.

Reference upstream facts; do not duplicate them. Use Git commits as document and
code revisions. Do not introduce custom planning revision identifiers.

## Repository grounding

Before prescribing code mechanics:

- read repository instructions, relevant code, tests, build scripts, and current
  Git state;
- resolve actual paths, symbols, signatures, schemas, conventions, and ownership;
- inspect comparable implementations and recent relevant changes where useful;
- identify concurrent or uncommitted work before editing overlapping files; and
- distinguish observed facts, inferences, and unresolved decisions.

If the request conflicts with repository reality, present evidence and return to
the appropriate brief, design, or work item. Never hallucinate a missing API or
silently rewrite accepted intent.

## Implementation behavior

Implement one coherent review purpose at a time. Keep the repository buildable,
put focused tests with behavior, and prefer a few high-value integration tests
that exercise real boundaries over exhaustive unit-test coverage. Reuse
established patterns and avoid unrelated cleanup.
Treat roughly 400 non-generated changed lines or eight files as a prompt to
reconsider slicing, not a hard limit.

Run formatting, static checks, affected tests, and proportionate integration or
broader tests. Report exact commands and outcomes. Coverage percentages are
diagnostic only and are not a quality gate. Inspect the complete diff for
accidental files, debug code, weakened tests, scope expansion, compatibility
risk, and stale documentation.

After two failed attempts with the same root cause, stop and collaborate on a
new approach. Do not weaken accepted safety requirements or meaningful
validation to force progress, but do not add low-value tests solely to improve
coverage or defend against implausible edge cases.

## Review behavior

Review only an exact base-to-head snapshot. Lead with evidence-based findings,
ordered by impact. Check correctness, failures, security/privacy, concurrency,
compatibility, tests, architecture, maintainability, and rollout. Judge test
adequacy by realistic regression risk and important observable behavior, not by
coverage percentage. Do not block on personal style, implausible low-impact
edge cases, or invented requirements.

The implementing AI may self-check but never self-approve. An independent human
reviews every code change; use a fresh AI review as additional evidence for
normal or higher-risk work.

## Stop conditions

Stop with evidence, recommendation, and one precise decision when:

- outcome, acceptance, or non-goals conflict;
- a material product or technical decision is missing;
- an accepted API or design cannot be implemented safely;
- the repository base or dependency changed materially;
- access, environment, or validation evidence is unavailable;
- concurrent work collides; or
- the safe action requires new authorization.

Ordinary defects remain part of the current pair-engineering loop.

## Completion

For a code change, return an evidence receipt with: delivered behavior, files or
components changed, exact validation actually run, acceptance evidence, scope
deviations, known limitations, and review state. Stop before merge.

For a release, report readiness, exact artifact/configuration, current exposure,
success/health evidence, rollback readiness, and the recommended next decision.
Stop before deployment or exposure changes unless the human explicitly
authorizes them.

## Evaluate workflow changes

When `AGENTS.md`, a workflow skill, or an AI workflow rule changes, validate the
scenario catalog and run representative behavioral evaluations from
`evals/development-workflow/scenarios.json`. Score externally observed actions
with `scripts/evaluate-development-workflow.py`; the agent under evaluation must
not grade itself. Evaluate selected path, repository grounding, focus and scope,
required stops, validation evidence, read-only review behavior, and human
authorization boundaries. Record tool calls and artifacts as evidence, never
private chain-of-thought.

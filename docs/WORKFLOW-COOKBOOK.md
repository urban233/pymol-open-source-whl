# Human-AI Development Workflow Cookbook

This cookbook gives copyable starting recipes for the four most common kinds of
software work. It assumes the repository policy in [AGENTS.md](../AGENTS.md) and
the [Product Development Workflow](for-human/development-guide.md).

The recipes are starting points, not substitutes for judgment. Risk overrides
change size: security, privacy, permissions, public contracts, persistent data,
billing, compliance, destructive behavior, or difficult rollback require
explicit design and independent review.

## Quick routing table

You can state any of these requests in normal language. The table explains the
AI's internal routing; it is not a list of commands the developer must memorize.

| Situation | Start with | Normal artifact |
|---|---|---|
| New greenfield project | `specify-project` | accepted `SPECIFICATION.md` |
| Single brownfield feature | `define-product` | feature brief; design if material |
| New product version with several developers | `define-product` or accepted product specification, then `design-solution` and `plan-delivery` | release brief/design plus delivery plan |
| Small local bug fix | `build-change` | issue plus code/tests |

All implementation eventually uses `build-change`, independent human review,
and proportionate validation. Use `review-change` as a fresh AI review for normal
or higher-risk work, `clean-code-review` for a focused coding-practices scan,
`pr-review` for an existing GitHub Pull Request and validated inline comments,
and `critique-review` for read-only suggested diffs from concrete findings,
and `launch-product` for material production exposure.

## Recipe 1: Create a greenfield project

### Use this recipe when

There is a product idea but no accepted product frame, architecture, or
repository implementation. The desired outcome is one coherent project
blueprint created through a guided discussion.

Do not use this recipe for a one-feature addition to an established product.

### Result

```text
Idea
  -> one-question-at-a-time specification interview
  -> accepted product frame
  -> accepted technical design
  -> canonical SPECIFICATION.md
  -> delivery plan for the first milestone
  -> repository bootstrap and small implementation slices
  -> staged launch and learning
```

### Step 1: begin the interview

Give the AI any existing notes, research, constraints, sketches, or policies.
Then use:

```text
Use $specify-project for this greenfield product. Interview me exactly one
question at a time and include your recommended default with every question.
Challenge unsafe or vague answers. Accept the product frame before designing the
technical system, then create one canonical SPECIFICATION.md. Do not create an
implementation roadmap, task list, or code.

The initial idea is: <describe the problem or opportunity>.
```

The AI should first settle the user problem, evidence, outcome, measures, V1
scope, non-goals, constraints, and assumptions. A technology such as Python,
Kubernetes, or an LLM is not by itself a product problem.

### Step 2: accept the product frame

Expect a concise summary and one explicit acceptance question. Check:

- users and problem are specific;
- evidence and assumptions are distinguished;
- success measures have a baseline or baseline plan;
- V1 is a complete, bounded outcome;
- non-goals block likely scope creep;
- safety, accessibility, legal, cost, and platform constraints are visible.

Say “yes” only if this is the product you want designed. Otherwise correct the
one material issue and continue the interview.

### Step 3: design the technical blueprint

The same interview now covers applicable architecture, data, contracts, clients,
security/privacy, failure modes, capacity, deployment, operations, testing,
migration, rollout, rollback, and alternatives.

The AI should recommend defaults but must not invent scale, policies, targets, or
organizational constraints. Ask for specialist human review where required.

### Step 4: accept and validate the specification

For a single-product repository, use `SPECIFICATION.md` at the root. For a
monorepo, use `docs/product/<slug>/SPECIFICATION.md` or its established
convention.

Before marking it Accepted:

- both `Product frame` and `Technical design` are Accepted;
- no blocking decision remains;
- every non-blocking unknown has an owner, evidence action, and decision point;
- components and contracts have owners;
- acceptance behavior traces to test and rollout evidence;
- required domain reviewers completed their review;
- the responsible human accepts planning against the exact file.

Run:

```text
python .agents/skills/specify-project/scripts/validate_specification.py SPECIFICATION.md
```

The validator checks structure, not whether the architecture is correct.

### Step 5: plan only the next delivery wave

```text
Use $plan-delivery with the accepted SPECIFICATION.md. Create outcome-based
milestones for the whole product, but decompose only the first milestone into
Ready work. Name owners, independent reviewers, contracts, simple dependencies,
integration checkpoints, and validation evidence.
```

### Step 6: bootstrap and build

Use the appropriate repository handbook:

- [Python Project Handbook](handbooks/PYTHON-PROJECT-HANDBOOK.md)
- [Language-Agnostic Project Handbook](handbooks/LANGUAGE-AGNOSTIC-PROJECT-HANDBOOK.md)

Then take one Ready item at a time:

```text
Use $build-change for <work-item>. Inspect the repository before planning, pair
with me on one reviewable vertical slice, run the documented validation, and
stop for any material decision not settled by SPECIFICATION.md.
```

Use a fresh `review-change` pass and an independent human reviewer. Do not ask AI
to implement the full specification in one pass.

If the review identifies a concrete bounded correction, use `critique-review` to
draft a suggested diff. It must not modify files; explicitly hand the accepted
suggestion to `build-change` or ask the developer to apply it, then repeat
independent review.

### Step 7: launch and learn

```text
Use $launch-product to assess the exact release candidate against
SPECIFICATION.md. Build a readiness evidence table, rollout stages, success and
guardrail thresholds, observation windows, rollback, ownership, and cleanup.
Do not deploy or expand exposure without human authorization.
```

Review product outcomes after exposure. Keep, improve, roll back, or stop based
on evidence.

## Recipe 2: Add one feature to a brownfield project

### Use this recipe when

An existing product and repository already work, and the goal is one bounded
user-visible capability. Existing architecture, conventions, ownership, and
production behavior constrain the solution.

### Result

```text
Repository inspection
  -> accepted feature brief
  -> optional feature design
  -> one or more small build/review loops
  -> proportionate rollout
```

### Step 1: frame the feature

```text
Use $define-product for this brownfield feature. Read the existing product and
repository documentation first. Produce a bounded feature brief with users,
outcome, acceptance scenarios, measures, first-release scope, non-goals,
constraints, assumptions, and risk. Do not design or implement it yet.

Feature request: <describe the requested behavior>.
```

Save the accepted brief at `docs/features/<slug>/brief.md` or the repository's
existing location.

### Step 2: decide whether a design is necessary

Skip a separate design when the implementation is local, reversible, and uses an
existing pattern without a material contract or risk decision.

Use `design-solution` when the feature changes:

- public or shared APIs;
- persistent data or migration;
- authentication, permissions, privacy, or abuse controls;
- cross-component ownership or orchestration;
- dependency, deployment, capacity, reliability, or rollback behavior.

Prompt:

```text
Use $design-solution for the accepted feature brief. Inspect current components,
callers, schemas, tests, deployment, ownership, and comparable code before
proposing a solution. Recommend the smallest compatible design, compare real
alternatives, and stop for material API, data, security, cost, or ownership
decisions.
```

### Step 3: plan or start the first change

For one developer and a small feature, proceed directly to a bounded work item.
For several developers or components, use `plan-delivery` first.

```text
Use $build-change for the first accepted feature slice. Ground the plan in the
current repository and accepted brief/design, keep the change reviewable, put
tests with behavior, and run proportionate validation.
```

### Step 4: review the exact change

```text
Use $review-change on the exact current diff against the accepted feature brief
and design. Do not modify files. Check correctness, failures, security/privacy,
compatibility, tests, architecture, maintainability, and rollout. Report only
actionable evidence-based findings.
```

An independent human reviews the complete diff and decides whether it may merge.
Re-run required checks after updating the target branch.

### Step 5: release proportionately

A low-risk internal feature may use the ordinary release path. User-facing,
stateful, permission-sensitive, or hard-to-reverse changes need `launch-product`
with flags or staged cohorts, defined evidence, and tested rollback.

Do not create a whole-project `SPECIFICATION.md` merely for this feature unless
the work actually changes the whole product blueprint.

## Recipe 3: Deliver a new version with multiple developers

### Use this recipe when

An established product is preparing a substantial release or new major version
with several developers, multiple components, shared contracts, migration, or
coordinated rollout.

Do not create a long-lived “version branch” by default. Prefer small compatible
changes integrated into protected `main`, with inactive flags or safe migration
states until exposure.

### Result

```text
Accepted release outcome
  -> accepted design/contracts
  -> outcome milestones and next wave of Ready work
  -> parallel work against stable contracts
  -> frequent integration and independent review
  -> release-candidate evidence
  -> staged migration and rollout
```

### Step 1: establish the release outcome

If the current product specification already covers the new version, reference
it and create a bounded release or feature brief for what changes. If the new
version is a whole-product redesign, revise it with `specify-project` and obtain
both acceptance checkpoints again.

Normal prompt:

```text
Use $define-product for version <version>. Inspect the current product documents
and production evidence. Define the changed outcome, users, acceptance,
compatibility promise, migration constraints, success measures, non-goals, and
release guardrails. Do not create implementation tasks.
```

### Step 2: accept shared design and contracts

```text
Use $design-solution for the accepted version brief. Inspect the current system
and define the minimum architecture changes, component ownership, APIs/schemas,
compatible intermediate states, contract fixtures, failure behavior,
observability, migration ordering, rollout, rollback, and cleanup.
```

Agree shared interfaces before parallel work. Each contract needs one owner,
consumer reviewers, a schema or signature, compatibility rules, and an
independently runnable fixture or contract test.

### Step 3: create the delivery plan

```text
Use $plan-delivery for version <version> with <developer names or teams>. Create
outcome-based milestones and only the next wave of Ready work. Give every item
one owner and a different reviewer. Record Blocked by, Integrates with, or Lands
after only when it changes action. Name integration checkpoints and an
integration owner where streams meet. Default to one active item per developer.
```

Good milestones demonstrate integrated user/system behavior. “Backend complete”
or “all components coded” are not outcome milestones.

### Step 4: run the team loop

For each developer:

1. Pull one Ready item.
2. Use `build-change` to inspect, plan, implement, and validate one small slice.
3. Use a fresh `review-change` pass for normal or higher risk.
4. Request the named independent human reviewer.
5. Update from `main`, revalidate, merge, and confirm postsubmit remains green.
6. Integrate against the accepted contract at the named checkpoint.
7. Pull the next item only when review and integration capacity exists.

Coordinate shared schemas, generated files, migrations, lockfiles, and registries
with one temporary owner or explicit landing order. Never resolve a semantic
conflict mechanically.

### Step 5: demonstrate and plan the next wave

At each milestone:

- show the real integrated behavior;
- compare it with acceptance and guardrails;
- review defects, operational evidence, and changed assumptions;
- update the canonical design only when reality changed;
- prepare the next wave of Ready items;
- stop work whose premise failed.

### Step 6: prepare and launch the version

```text
Use $launch-product for the exact version <version> release candidate. Verify
functional, contract, migration, security/privacy, performance, operability,
artifact, support, and rollback evidence. Define internal, canary, and expansion
stages with thresholds, observation windows, decision owners, and cleanup. Do
not deploy, migrate, or expand exposure without human authorization.
```

Build once from a known green commit and promote the same immutable artifact.
Rehearse migration and rollback at representative fidelity. Complete the release
only after production and product evidence meet the accepted decision rules.

## Recipe 4: Fix a small bug

### Use this recipe when

The defect is local, understood or reproducible, reversible, and does not require
a material product, API, data, security, privacy, or architecture decision.

### Result

```text
Issue -> reproduce -> failing regression test -> minimal fix
      -> focused and broader validation -> human review -> merge
```

### Step 1: write the issue

Record:

- observed behavior;
- expected behavior;
- reproduction steps and environment;
- impact and affected versions;
- logs or screenshots with sensitive data removed;
- a concise non-goal;
- rollback or disable mechanism if production is currently affected.

Do not write a product brief or specification for an ordinary bug.

### Step 2: diagnose and fix interactively

```text
Use $build-change for this bug. Inspect the repository and current Git state,
reproduce the failure, identify the root cause, add a regression test that fails
for the bug when practical, implement the smallest coherent fix, run affected
and proportionate broader checks, and inspect the complete diff. Stop if the bug
reveals a material product, API, data, security, or architecture decision.
```

The regression test should assert externally meaningful behavior, not merely the
chosen implementation. Preserve unrelated local changes.

### Step 3: review

Every code change receives independent human review. Use `review-change` when
the bug is normal or higher risk, the root cause is subtle, concurrency or
security is involved, or a second AI pass would materially improve confidence:

```text
Use $review-change on the exact bug-fix diff. Do not modify code. Verify the root
cause, regression coverage, failure paths, compatibility, and absence of
unrelated change.
```

### Step 4: merge and verify

- update from the target branch and rerun required checks;
- merge through the protected path;
- confirm postsubmit is green;
- verify the repaired behavior in the appropriate environment;
- use the existing release or hotfix procedure;
- monitor the original failure signal.

If the fix needs a risky migration or user-exposure decision, stop treating it as
a small bug and use `design-solution` plus `launch-product` controls.

## Final rule

The safest workflow is not the one with the most documents. It is the lightest
workflow that makes intent, risk, evidence, ownership, review, and release
authority explicit for the change being made.

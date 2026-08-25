# Idea-to-Production Handbook for Human-AI Software Development

**Audience:** beginners who need a guided path, working developers who need an
operating model, and senior engineers or managers establishing a multi-developer
delivery system

**Purpose:** explain exactly how to turn an idea into a maintained production
product while humans and AI collaborate closely, without relying on unattended
coding loops or replacing engineering judgment

**Standard:** this handbook combines six lifecycle skills and an optional guided
project-specification facade with publicly documented Google engineering
practices: clear problem definition,
design review for consequential systems, canonical documentation, explicit
ownership, small change lists, independent review, continuous integration,
progressive delivery, and learning from production. Internal Google or DeepMind
processes that are not public are not presented as facts.

## 1. The operating model

Developers interact with one four-step workflow: **Understand**, **Build**,
**Review**, and **Ship**. They describe the work normally; the AI selects the
applicable skills. Product definition, technical design, and delivery planning
deepen Understand only when uncertainty, risk, or team coordination requires
them.

Software delivery is a learning loop, not a document assembly line:

```text
Observe a problem
  -> define the outcome
  -> design the smallest safe solution
  -> plan the next useful milestone
  -> build and review small changes
  -> release gradually
  -> measure real behavior
  -> keep, improve, or stop
```

The lifecycle skills guide the parts of that loop. A greenfield project can use
the optional facade shown on the left:

```text
Idea
  -> specify-project      Guided product frame + design -> SPECIFICATION.md
     OR
  -> define-product       Modular why, users, outcome, scope, success
  -> design-solution      Modular architecture, interfaces, trade-offs, risk
  -> plan-delivery        Milestones, ready work, owners, dependencies
  -> build-change         Inspect, plan, implement, and validate one item
  -> review-change        Independently evaluate the exact change
  -> pr-review            Review an existing GitHub PR with anchored comments
  -> clean-code-review    Optionally scan coding practices and cataloged smells
  -> critique-review      Optionally prepare precise suggested diffs
  -> launch-product       Prove readiness, stage exposure, learn, and clean up
```

The facade does not introduce a seventh lifecycle stage. It combines product
definition and solution design into one continuous interview and one canonical
artifact while keeping their human acceptance decisions separate. None of these
are mandatory documents for every edit; invoke the relevant forms of thinking in
proportion to uncertainty and consequence.

## 2. Principles that do not change with team size

1. **Start with a user problem.** Output is not the outcome; code is not proof of value.
2. **Use one source of truth.** Each fact has one canonical owner and location.
3. **Make risk visible early.** Security, privacy, data, APIs, money, and irreversible
   operations are design concerns, even when their diffs are small.
4. **Keep decisions human.** AI may recommend; accountable people accept product,
   architectural, risk, merge, and release decisions.
5. **Inspect before proposing.** Plans must cite the actual repository and system.
6. **Work in small vertical slices.** Prefer usable, testable behavior over layers
   completed in isolation.
7. **Test the claim, not the implementation.** Evidence must connect to acceptance
   behavior and realistic failure modes.
8. **Separate author and reviewer.** The implementing agent cannot approve itself.
9. **Keep the main branch healthy.** Integrate continuously and restore green quickly.
10. **Release is a controlled experiment.** Observe defined signals and be ready to stop.
11. **Operate what you ship.** Ownership continues through incidents, maintenance,
    deprecation, and deletion.
12. **Improve the system from evidence.** Add process only for an observed failure mode.

## 3. Select the lightest safe path

### 3.1 Quick change

Use for a local, reversible, well-understood fix or refactor with no material
contract, data, security, privacy, or operational decision.

```text
Issue -> build-change -> human review -> merge -> ordinary release
```

Minimum record: problem, acceptance behavior, non-goal, risk, diff, test evidence,
and review.

### 3.2 Feature

Use for bounded user-visible behavior or a change spanning several files or
components.

```text
define-product -> optional design-solution
  -> one or more build-change/review-change loops
  -> proportionate launch
```

Create a design when the feature introduces a meaningful technical decision,
shared interface, migration, new dependency, permission, sensitive data,
reliability burden, or difficult rollback.

### 3.3 Product or program

Use for a new product, cross-team system, platform, significant migration, or
high-consequence work.

```text
Guided: specify-project -> plan-delivery
    OR
Modular: define-product -> design-solution -> plan-delivery
  -> repeated build-change/review-change loops
  -> launch-product -> post-launch learning
```

Choose `specify-project` when a greenfield or whole-product blueprint benefits
from a long, recommendation-led interview and one `SPECIFICATION.md`. Choose the
modular path when product and design have different owners or review cycles, or
when an existing product already has a canonical brief and architecture.

Plan the entire product at milestone resolution, but decompose only the next
milestone into ready work. This rolling-wave approach accommodates discovery
without abandoning accountability.

### 3.4 Risk overrides size

Require explicit design and independent specialist review for changes involving:

- authentication, authorization, identity, cryptography, or secrets;
- personal, confidential, regulated, or high-value data;
- payments, billing, entitlements, quotas, or legal commitments;
- persistent schemas, migrations, deletion, or destructive operations;
- public APIs, protocols, model formats, or compatibility guarantees;
- untrusted code/content execution or powerful AI tool access;
- material availability, safety, compliance, or reputation risk.

A three-line permission change can deserve more rigor than a 500-line internal
refactor.

## 4. Roles and authority

One person may hold several roles, but the responsibilities remain distinct.

| Role | Accountable for | May delegate to AI | Must remain human-controlled |
|---|---|---|---|
| Product owner | problem, priority, scope, success | research synthesis, draft criteria | accepted outcome and scope |
| Tech lead/design owner | architecture, contracts, technical risk | repository analysis, options, prototypes | accepted material trade-offs |
| Work-item owner | delivery of one bounded change | implementation and validation | scope changes and final handoff |
| Reviewer | independent code-health judgment | second-pass analysis | approval or required changes |
| Security/privacy specialist | domain risk and policy | evidence gathering, threat prompts | risk acceptance/exceptions |
| Release owner | readiness and exposure | checklist, monitoring summary | deploy/expand/rollback authorization |
| Operator | production health and incidents | diagnosis, correlation, draft runbook | consequential remediation authority |

For multi-developer work, name the product owner, design owner, component owners,
and release owner. Name an integration owner only where several streams meet.

For solo work, write down which role you are playing and deliberately switch
contexts. A fresh AI review is useful, but it is not a substitute for specialist
human review when the consequences require one.

## 5. Stage 0: discover and frame the opportunity

### Goal

Decide whether a real problem is worth solving before optimizing a solution.

### Inputs

- user conversations, support cases, field observation, or workflow recordings;
- product analytics and reliability data;
- organizational strategy and constraints;
- competitor or alternative analysis;
- legal, security, privacy, accessibility, and operational context.

### Procedure

1. Write the observed problem without naming a solution.
2. Identify the affected user and the situation in which the problem occurs.
3. Record frequency, severity, and current workaround.
4. State the desired change in user or system behavior.
5. Identify a measurable leading indicator and a durable outcome measure.
6. List constraints and risks that could invalidate the idea.
7. Distinguish evidence from assumption.
8. Select the cheapest test that could disprove the most important assumption.

### AI use

AI can cluster interview notes, identify contradictions, draft alternative problem
statements, enumerate stakeholders, and propose testable hypotheses. Provide
de-identified material and verify quotations or source claims. Do not let AI
invent user evidence.

### Exit criteria

- a specific user and problem are supported by evidence;
- the desired outcome can be observed;
- the next experiment or product-definition decision has an owner;
- a stop condition exists for an idea that is not supported.

## 6. Stage 1: define the project, product, or feature

Choose one artifact strategy before writing. Never create a combined
specification plus a brief and design that repeat the same facts.

### 6.1 Guided greenfield specification

Invoke `specify-project` for a new product, whole-system redesign, or explicit
request for a comprehensive `SPECIFICATION.md`. The skill is deliberately
conversational:

- it inspects supplied material and an existing repository before asking for
  discoverable facts;
- it asks exactly one targeted question per interview response;
- every question includes a recommended default and material trade-offs;
- it labels verified facts, accepted decisions, assumptions, and open items;
- it periodically summarizes progress without widening scope;
- it refuses to invent user evidence, numerical targets, scale, policy, or
  architectural constraints.

The interview has two checkpoints:

1. **Product frame:** problem, evidence, users, outcomes, measures, guardrails,
   scenarios, V1 scope, non-goals, constraints, and assumptions.
2. **Technical design:** components, owners, data and state, contracts, clients,
   security/privacy, failure behavior, capacity, deployment, observability,
   evaluation, compatibility, rollout, rollback, and alternatives.

The product frame must be accepted before detailed technical design. The overall
specification becomes Accepted only after both checkpoints, required specialist
reviews, closure of blocking decisions, a consistency review, and explicit human
acceptance of the exact file.

Use `SPECIFICATION.md` at the root of a single-product repository. In a monorepo
or multi-product repository, use `docs/product/<slug>/SPECIFICATION.md` or the
established convention. Git records revisions. The specification is durable and
maintained, not frozen forever.

It is complete enough for planning when all delivery-blocking decisions are
resolved and every remaining non-blocking unknown has an owner, evidence action,
and decision point. It intentionally contains no sprint plan, implementation
roadmap, task checklist, staffing allocation, or code.

After acceptance, continue directly to `plan-delivery` for multi-developer or
multi-milestone work, or `build-change` for one bounded first slice. Do not run a
separate `define-product` or `design-solution` pass unless the artifact strategy
is deliberately changed.

### 6.2 Modular product or feature definition

Invoke `define-product` when the idea needs to become an agreed brief. This skill
does not design the technical solution.

### 6.3 Build the brief

Create one canonical brief containing:

1. **Status and ownership:** Draft or Accepted, owner, reviewers, date.
2. **Problem:** current situation and evidence.
3. **Users:** primary users, affected non-users, and accessibility needs.
4. **Outcome:** the changed behavior or state, not shipped components.
5. **Success measures:** baseline, target, measurement source, time window, owner.
6. **Guardrails:** safety, reliability, privacy, cost, and quality limits.
7. **In scope:** capabilities needed for the outcome.
8. **Non-goals:** plausible work explicitly excluded.
9. **Acceptance scenarios:** observable Given/When/Then behavior or equivalent.
10. **Constraints:** deadline, policy, platform, budget, compatibility, operations.
11. **Assumptions and unknowns:** with validation method and owner.
12. **Workflow path:** quick change, feature, or product, with risk rationale.

### 6.4 Write useful success measures

Use this form:

```text
Measure: percentage of invited users who complete setup unaided
Baseline: 41% over the previous 28 days
Target: at least 65%
Guardrail: support contacts do not rise by more than 5%
Window: 14 days after each cohort receives the feature
Source: product analytics dashboard <link>
Owner: product owner
Decision: expand if target and guardrail hold; investigate or stop otherwise
```

"Launch the dashboard" is output. "Reduce median investigation time from 20 to
8 minutes without increasing incorrect decisions" is an outcome.

### 6.5 Review the brief

Ask reviewers to find ambiguity, not polish prose. Resolve:

- conflicting acceptance scenarios;
- measures that cannot be collected;
- missing user groups or abuse cases;
- hidden migration or compatibility obligations;
- scope that is too broad for one useful milestone;
- assumptions being presented as facts.

The product owner marks the brief Accepted. Acceptance authorizes design or
implementation exploration, not release.

### 6.6 Solo versus team

**Solo:** time-box the brief to one page for normal work. Ask AI to challenge the
problem statement and non-goals. Obtain domain input when the problem is not your
own.

**Team:** review asynchronously first; meet only to resolve disputed choices.
Record the accepted outcome once. Link issues and designs to it rather than
copying the text.

## 7. Stage 2: design the solution

Invoke `design-solution` for a significant feature, product, migration, shared
contract, or high-risk change on the modular path. Skip it for an obvious local
change and when an accepted combined `SPECIFICATION.md` already contains the
technical blueprint.

### 7.1 Begin with repository and system inspection

Before proposing architecture, the developer and AI inspect:

- current components, entry points, and dependency boundaries;
- existing interfaces, schemas, migrations, and compatibility policy;
- test layout, build commands, CI, deployment, and feature flags;
- ownership, adjacent active work, and previous decisions;
- production constraints, SLOs, incident history, and capacity signals;
- security/privacy classifications and trust boundaries.

Label findings as **verified**, **inferred**, or **unknown**. Link verified claims
to code, configuration, dashboards, issues, or authoritative documentation.

### 7.2 Develop alternatives

For each credible option, compare:

- how it satisfies the outcome and acceptance scenarios;
- component and ownership changes;
- public and internal interfaces;
- data lifecycle and migration;
- security, privacy, abuse, and compliance effects;
- failure behavior, reliability, capacity, and observability;
- testability and evaluator independence;
- rollout, compatibility, and rollback;
- implementation and long-term maintenance cost;
- reversibility and unresolved assumptions.

Include "do nothing", configuration/process change, and reuse of an existing
capability when credible. Do not stage a fake comparison in which only one
option is viable.

### 7.3 Write the design

The accepted design normally contains:

1. context and links to the accepted brief;
2. goals and non-goals inherited from the brief;
3. current-system facts;
4. proposed component boundaries and owners;
5. request, event, and data flow;
6. interfaces, schemas, validation, and error semantics;
7. dependency choices;
8. security, privacy, abuse, and compliance analysis;
9. reliability, capacity, concurrency, and resource behavior;
10. observability and operational ownership;
11. test and evaluation strategy;
12. migration, compatibility, rollout, and rollback;
13. alternatives and reasons for rejection;
14. unresolved questions, decisions, owners, and dates.

Use diagrams where relationships are hard to express linearly. A diagram must
have labels, trust or ownership boundaries where relevant, and prose explaining
the important behavior.

### 7.4 Review and accept the design

Invite the smallest set of people who can find consequential errors: component
owners and, when applicable, security, privacy, data, storage, reliability,
accessibility, localization, or legal specialists.

Reviewers answer:

- Can the system meet the accepted behavior?
- Are component ownership and dependency direction coherent?
- Are interfaces precise enough for independent implementation?
- Does failure remain bounded and observable?
- Can data and APIs migrate without trapping consumers?
- Can the release be stopped or reversed safely?
- Is the design simpler than the problem warrants?

The design owner records decisions and marks the document Accepted. Open
questions that change safety or interfaces must be closed before dependent work
is ready.

### 7.5 AI-specific design controls

If the product itself uses an AI model, specify:

- model purpose, version policy, context sources, and allowed tools;
- data classification, retention, training use, and regional requirements;
- prompt-injection and untrusted-content boundaries;
- deterministic validation around model output;
- offline evaluation set and ownership;
- quality, safety, latency, availability, and cost thresholds;
- human escalation and fallback behavior;
- monitoring for regressions, drift, abuse, and unexpected tool use;
- rollback across prompt, model, retrieval corpus, tool, and application versions.

Never use "the model will handle it" as an interface or safety strategy.

## 8. Stage 3: plan delivery

Invoke `plan-delivery` after either the combined specification or the modular
brief and required design are accepted when work involves multiple developers,
multiple milestones, or significant coordination.

### 8.1 Define outcome-based milestones

A milestone proves an integrated capability, not completion of an organizational
layer.

Good:

```text
M1: An internal user can create, retrieve, and delete one test notification
    preference through the real service boundary, with audit evidence.
```

Weak:

```text
M1: Backend complete.
```

For each milestone define:

- demonstrated behavior;
- entry and exit evidence;
- relevant success or guardrail signal;
- owner and expected review specialties;
- risks retired and assumptions tested;
- rollout scope, if any.

### 8.2 Create ready work items

Each item should be small enough for one owner and one focused pull request or a
short sequence of explicitly linked changes. It includes:

```text
Title
Outcome and user/system behavior
Acceptance scenarios
In scope / non-goals
Repository areas likely involved
Accepted interface or design links
Risk and required reviewers
Validation commands and expected evidence
Rollout/compatibility notes
Owner and independent reviewer
Blocked by / Integrates with / Lands after
```

An item is Ready when material behavior and interface decisions are accepted,
dependencies are available or mocked by an agreed contract, validation is
possible, and the owner can begin without guessing.

### 8.3 Manage dependencies simply

Use only the relation needed to change action:

- **Blocked by:** safe work cannot start.
- **Integrates with:** work can proceed against an accepted contract; name the
  integration checkpoint and owner.
- **Lands after:** ordering matters for merge, migration, or release.

Avoid a second graph if the tracker already represents these relations.

### 8.4 Control work in progress

Default to one active implementation item per developer. Review and integration
capacity constrain throughput more often than idea supply. When blocked, help
clear the block or review another change rather than starting several items.

Reserve capacity for review, integration, defects, and operational work. A plan
that assigns every developer 100 percent to feature coding is not credible.

### 8.5 Plan in waves

Fully prepare the current milestone. Keep later milestones at outcome level until
evidence reduces uncertainty. At every milestone demonstration:

1. show working behavior;
2. compare evidence with acceptance and guardrails;
3. inspect integration and operational risks;
4. update the design only if reality changed;
5. prepare the next wave;
6. stop or redirect work whose premise failed.

## 9. Stage 4: implement one bounded change

Invoke `build-change` for hands-on pairing. It follows six explicit steps.

### 9.1 Frame

Confirm in a few sentences:

- desired behavior and acceptance scenarios;
- non-goals;
- risk level;
- accepted brief/design/work-item links;
- human decisions that must not be inferred.

If these conflict, stop and repair the source document before editing code.

### 9.2 Inspect

AI reads the repository instructions, relevant code, callers, tests, build and CI
configuration, and nearby conventions. It identifies concurrent or uncommitted
work without overwriting it. It reports facts, assumptions, and the smallest
likely change surface.

### 9.3 Plan

For a normal change, agree a short implementation plan:

1. behavior or contract change;
2. implementation files and boundaries;
3. tests that establish acceptance and negative behavior;
4. documentation, telemetry, migration, or rollout updates;
5. exact validation commands.

Use a persisted implementation plan only for complex, risky, interrupted, or
cross-session work. The issue is sufficient for a small change.

### 9.4 Implement

- Change one coherent slice.
- Follow existing architecture and style unless the accepted design changes it.
- Preserve unrelated user changes.
- Keep entry points thin and failure behavior explicit.
- Add tests with behavior, not after a giant implementation.
- Add dependencies only with explicit rationale.
- Surface material discoveries before changing scope or design.
- Keep temporary compatibility or flag code owned and time-bounded.

AI shares concise updates at meaningful checkpoints. It should not ask for
approval of routine, reversible edits inside the accepted plan.

### 9.5 Validate

Run the narrowest useful checks while iterating, then the repository's complete
presubmit before handoff:

```text
formatter check
lint/static/type/API checks
affected unit and contract tests
relevant integration or end-to-end tests
artifact build/package check
security or migration checks required by risk
complete diff inspection
```

Record exact commands, result, and any check that could not run. Do not describe
an unexecuted test as passing.

### 9.6 Handoff

Provide:

- outcome implemented;
- files and contracts changed;
- validation evidence;
- remaining risks or assumptions;
- rollout/compatibility notes;
- exact snapshot or commit to review.

The human examines the diff. Commit, push, and pull-request creation follow the
repository's explicit authorization policy.

### 9.7 Stop conditions

AI stops and asks one precise question when:

- accepted requirements conflict;
- an API, data, security, dependency, or architecture choice is missing;
- the target files changed unexpectedly or concurrent work collides;
- the requested behavior cannot be validated;
- implementation reveals materially different scope or risk;
- required access or human authority is absent.

It should state verified facts, its recommendation, and the consequence of each
reasonable option.

## 10. Stage 5: independently review each change

Invoke `review-change` in a fresh context for normal or higher-risk work. The
review skill is read-only unless a later request explicitly asks for fixes.

When a finding needs a concrete correction, invoke `critique-review` to prepare
a precise suggested diff. It remains read-only; explicitly hand an accepted
suggestion to `build-change` or ask the developer to apply it, then repeat the
fresh independent review.

### 10.1 Freeze the review target

Identify the pull request head, commit, patch, or working-tree snapshot. A review
of an earlier diff is not approval of later edits. Re-review material changes and
revalidate after updating the target branch.

### 10.2 Inspect in risk order

1. accepted behavior and non-goals;
2. authorization, data exposure, destructive actions, and trust boundaries;
3. public contracts, migrations, compatibility, and rollback;
4. core logic, state transitions, concurrency, and failure handling;
5. tests and evaluator independence;
6. reliability, observability, performance, and resource limits;
7. maintainability, naming, comments, style, and documentation;
8. unrelated scope or generated noise.

### 10.3 Report findings precisely

Use severity consistently:

| Severity | Meaning |
|---|---|
| P0 | immediate catastrophic/security impact; stop release and escalate |
| P1 | serious correctness, security, data, or availability defect; must fix |
| P2 | real defect or significant maintainability/operability risk; normally fix |
| P3 | minor issue or improvement with limited impact |

Every finding includes a concise title, exact file/line, trigger, observed or
likely impact, and why current tests or controls do not prevent it. Do not bury
blocking defects among style suggestions. If no actionable finding exists, say
so and identify residual test or context limitations.

### 10.4 Human review standard

The responsible human reads the complete change and the AI review. Automation
and AI provide evidence; they do not authorize merge. The human decides whether
the change improves overall code health and satisfies policy.

For high-risk work, use two-person approval, domain specialists, restricted
evaluation data, or staging evidence as defined by policy.

## 11. Stage 6: integrate continuously

After approval:

1. update from the protected target branch;
2. resolve conflicts with the owning developers, not by guessing intent;
3. rerun required checks on the merge candidate;
4. merge through the approved mechanism;
5. verify postsubmit remains green;
6. revert or repair a broken head immediately;
7. delete the short-lived branch;
8. update tracker status from repository evidence.

Prefer small changes that keep `main` buildable and testable. For incomplete
features, use a safe disabled flag or compatibility path. Avoid branches that
defer integration for weeks.

## 12. Stage 7: prepare and launch

Invoke `launch-product` when code approaches real exposure.

### 12.1 Prove readiness

Review the exact release candidate for:

- accepted functional scenarios and non-goals;
- security, privacy, compliance, and accessibility sign-off;
- API/schema compatibility and migration rehearsal;
- capacity, performance, reliability, and dependency behavior;
- build provenance, licences, vulnerabilities, and artifact identity;
- logs, metrics, traces, dashboards, alerts, and support ownership;
- runbooks, backup/restore, rollback or forward-fix procedure;
- product analytics and experiment integrity;
- user/support documentation and communication;
- flag ownership and cleanup date.

Readiness is an evidence table, not a confident paragraph. Each item has a link,
owner, state, and exception decision.

### 12.2 Define rollout stages

For each stage state:

```text
Cohort/exposure: internal staff, then 1%, 10%, 50%, 100%
Artifact/config: immutable artifact digest and flag/config version
Start authority: release owner
Minimum observation: enough time/traffic for the chosen signals
Success: product and reliability thresholds
Guardrails: error, latency, safety, support, cost, data-quality thresholds
Stop: immediate pause conditions
Rollback: exact technical and communication action
Decision evidence: dashboard/query/incident link
```

Canary analysis compares the candidate with a meaningful baseline. Account for
low traffic, novelty, seasonality, and delayed failures. Absence of alerts is not
proof of success if the relevant user behavior was not measured.

### 12.3 Keep authority explicit

The release owner authorizes deployment, migration, user exposure, and expansion.
AI may watch dashboards, summarize evidence, and recommend an action. It must not
silently expand exposure or perform irreversible remediation.

### 12.4 Close the launch

A launch is complete when:

- the intended cohort has stable evidence for the required window;
- open exceptions have owners and dates;
- temporary flags, dual writes, compatibility paths, and migration tooling have
  cleanup items;
- documentation and support state are current;
- the team records whether the product outcome was met.

Merge is not launch. Full exposure is not proven product value.

## 13. Stage 8: operate, learn, and maintain

### 13.1 Monitor the right layers

Track:

- **product:** adoption, task success, retention, user time, error recovery;
- **quality:** correctness, defect escape, accessibility, model/evaluator quality;
- **reliability:** availability, latency, saturation, dependency failures, SLO burn;
- **security/privacy:** abuse signals, authorization denial, vulnerability age,
  sensitive-data handling;
- **delivery:** deployment frequency, lead time, change failure, recovery time;
- **cost:** infrastructure, third-party, model, storage, support, and developer time.

Metrics need an owner, definition, data source, and response. Avoid dashboards
that nobody uses to make a decision.

### 13.2 Respond to incidents

During an incident:

1. protect users and contain damage;
2. establish an incident lead and communication channel;
3. preserve a timeline and evidence;
4. use tested rollback or mitigation;
5. verify recovery through user-impact signals;
6. communicate status at defined intervals;
7. conduct a blameless review focused on system conditions;
8. assign corrective actions with owners and verification.

AI may correlate logs, search code, draft timelines, and propose hypotheses. A
human incident lead controls consequential actions and validates conclusions.

### 13.3 Feed learning back

At the outcome-review date, decide:

- **keep and scale** because outcome and guardrails hold;
- **iterate** because evidence identifies a repairable gap;
- **hold** because evidence is insufficient;
- **roll back** because harm or regression exceeds value; or
- **retire** because the premise failed or costs outweigh benefits.

Update the brief when the product outcome changes, the design when architecture
or contracts change, code/tests for behavior, and the tracker for current work.
Do not rewrite history to make the original prediction appear correct.

## 14. The day-to-day developer loop

### Beginning of day or work session

1. Read current milestone, work item, accepted design, and recent target-branch changes.
2. Check CI, incidents, dependency alerts, and review requests.
3. Select one Ready item; confirm owner and reviewer.
4. Ask AI to frame and inspect using `build-change`.
5. Agree today's smallest demonstrable slice and validation.

### During implementation

1. Pair with AI on one coherent change.
2. Run focused tests after each behavior boundary.
3. Commit small checkpoints locally if repository policy permits.
4. Surface unexpected contract or scope changes immediately.
5. Review another developer's small change while waiting on CI.
6. Keep the branch current enough to expose integration problems early.

### Before requesting review

1. Run the complete presubmit command.
2. Read the complete diff as if reviewing someone else's work.
3. Remove unrelated edits, debug output, and obsolete comments.
4. Update tests, docs, metrics, migration, and rollout material.
5. Write a concise change description and link canonical artifacts.
6. Run a fresh `review-change` pass for normal or higher risk.
7. Request the named human reviewer.

### End of day or handoff

1. Keep `main` green; do not merge a knowingly broken intermediate state.
2. Record only current status, evidence, block, and next action in the tracker.
3. Leave the branch and worktree in a reproducible state.
4. Escalate decisions that prevent safe continuation.
5. Do not create a second narrative status document.

## 15. Multi-developer coordination in detail

### 15.1 Divide by coherent ownership

Prefer work streams that align with components and contracts. Avoid dividing one
function among several people or assigning frontend/backend independently before
their behavior contract is accepted.

Every shared interface has:

- an accountable owner;
- consumer reviewers;
- an accepted schema or signature;
- fixtures or contract tests;
- compatibility and rollout rules;
- an integration checkpoint.

### 15.2 Sequence a typical feature

```text
Accepted brief
  -> accepted interface and migration design
  -> contract fixture/test lands
  -> producer and consumer slices proceed in parallel
  -> integration owner verifies real boundary
  -> end-to-end acceptance scenario
  -> staged release
```

Foundation work should be the smallest enabling change. Do not create a large
"platform first" program unless several proven consumers justify it.

### 15.3 Handle hotspots

Generated files, shared schemas, central registries, migrations, and dependency
manifests frequently conflict. Assign one coordination rule:

- a single temporary owner;
- ordered landing (`Lands after`);
- a preparatory contract change;
- or regeneration after preceding changes merge.

Never resolve a semantic conflict by accepting both sides mechanically.

### 15.4 Use a sustainable rhythm

- Product outcome and current milestone review: weekly or at evidence boundaries.
- Design review: when a material decision is ready, not as recurring ceremony.
- Work selection: pull one Ready item when capacity exists.
- Review: continuous, with explicit response expectations.
- Integration demonstration: every milestone.
- Retrospective: after a meaningful release or incident, focused on a few changes.

Status meetings should not reproduce information already visible in the tracker
and repository.

## 16. Solo-developer adaptations

A solo project can use the same controls with less coordination overhead:

| Team control | Solo equivalent |
|---|---|
| product review | short written hypothesis plus user evidence |
| design reviewers | fresh AI critique plus external specialist for high risk |
| delivery tracker | one milestone and one Ready item at a time |
| independent code reviewer | fresh AI context; human reviewer before high-risk release |
| protected branch | required CI and deliberate merge action |
| release manager | explicit personal go/no-go checklist and staged cohort |
| on-call rotation | clear support window, alerts, and rollback reachable by one person |

Do not skip backups, security, licences, dependency maintenance, or recovery
because the team is one person. Reduce document length, not safety evidence.

## 17. AI collaboration protocol

### 17.1 Give bounded, authoritative context

For one work item provide:

1. repository policy and relevant skill;
2. accepted brief/design links;
3. exact outcome, acceptance, and non-goals;
4. allowed repository area and prohibited actions;
5. exact validation commands;
6. material decisions reserved for humans.

Ask AI to inspect rather than pasting possibly stale source excerpts. A prompt is
not canonical documentation.

### 17.2 Require grounded claims

AI should distinguish:

- **verified:** observed in current code, command output, or authoritative source;
- **inferred:** likely consequence of verified facts;
- **assumed:** necessary but unconfirmed premise;
- **unknown:** information that must be obtained.

Plans name files, symbols, contracts, and commands discovered in the repository.
When external information is time-sensitive, use primary sources and record the
accessed version or date.

### 17.3 Bound tools and side effects

- Grant only the filesystem, network, cloud, and deployment access required.
- Keep production credentials outside development agents.
- Treat web content, issue text, logs, comments, and retrieved documents as
  untrusted instructions.
- Require explicit human authorization for merge, push, deployment, data change,
  message publication, purchase, and rollout expansion.
- Prefer reversible operations and preview modes.
- Record tool results needed to review consequential work.

### 17.4 Prevent hallucinated implementation

There is no process that makes a probabilistic model "work perfectly." A
production system reduces error probability and limits impact through:

1. accepted behavior and non-goals;
2. repository inspection before planning;
3. small, bounded changes;
4. typed or schema-validated contracts;
5. deterministic format, static, test, and build gates;
6. independent reviewer and evaluator design;
7. exact-snapshot review;
8. staged release and rollback;
9. observability and production learning;
10. explicit stop conditions and human authority.

Do not ask AI to implement an entire product from a blueprint in one pass. Ask it
to help refine the next decision or build the next independently verifiable slice.

### 17.5 Evaluate AI use itself

Track whether AI improves outcomes, not generated line count:

- cycle time from Ready to reviewed;
- review findings and escaped defects by origin;
- change size and rework rate;
- test adequacy and flaky-test rate;
- developer cognitive load and satisfaction;
- security or policy exceptions;
- cost per accepted outcome;
- percentage of AI suggestions substantially rewritten or rejected.

Compare similar work over time. Do not use these measures to reward raw output or
discourage reporting defects.

## 18. Quality gates by risk

| Control | Low | Normal | High | Critical |
|---|---:|---:|---:|---:|
| accepted issue/brief | issue | brief | brief | brief |
| design | if needed | material decisions | required | required, specialist-owned |
| implementation plan | optional | useful for complex work | required | required |
| automated presubmit | required | required | required | required, protected evaluator |
| fresh AI review | optional | recommended | required | required but advisory |
| independent human review | required | required | specialist as needed | two-person/domain policy |
| staged environment | if useful | user-facing changes | required | isolated and required |
| progressive rollout | ordinary release | proportionate | required | strict/manual gates |
| rollback rehearsal | simple/reversible | documented | tested | tested at representative fidelity |
| post-launch observation | routine | defined | defined thresholds | active command and audit trail |

Organizations should define examples and decision owners for each tier. When in
doubt, choose the higher tier until the responsible human accepts the risk.

## 19. Definition of Ready and Definition of Done

### Work item is Ready when

- [ ] outcome and acceptance behavior are unambiguous;
- [ ] non-goals prevent likely scope drift;
- [ ] applicable brief and design are Accepted;
- [ ] shared contracts and fixtures exist or are the item itself;
- [ ] dependencies and landing relations are explicit;
- [ ] owner and independent reviewer are named;
- [ ] validation is feasible;
- [ ] risk and specialist reviews are known;
- [ ] rollout or compatibility requirements are understood.

### Code change is Done when

- [ ] implementation satisfies acceptance behavior;
- [ ] unrelated work is absent;
- [ ] tests would catch realistic regressions;
- [ ] formatting, static checks, tests, and build pass on the exact snapshot;
- [ ] security, privacy, compatibility, migration, and operations are addressed;
- [ ] documentation and examples are current;
- [ ] complete diff has independent human review;
- [ ] the protected branch is green after integration.

### Feature is Done when

- [ ] code-change criteria hold for every constituent change;
- [ ] integrated acceptance behavior works in the target environment;
- [ ] release candidate, migration, telemetry, support, and rollback are ready;
- [ ] rollout completed under defined thresholds;
- [ ] product and guardrail evidence were reviewed;
- [ ] temporary controls have cleanup owners and dates.

"Done coding" is not a lifecycle state.

## 20. Worked example: notification preferences

### Idea

Users miss important alerts because they cannot control notification channel and
quiet hours.

### Brief

- User: account administrators receiving operational alerts.
- Outcome: administrators configure preferences without support assistance.
- Measure: setup completion rises from 45% to 75%; missed critical-alert reports
  do not increase.
- Scope: email/SMS choice and one daily quiet interval.
- Non-goals: marketing preferences, per-event rules, mobile push.
- Risk: quiet hours must never suppress critical security alerts.

### Design

- Preference API owns validated user settings and versioned schema.
- Notification service consumes a read-only preference contract.
- Security events bypass quiet hours through an explicit event classification.
- Existing users default to current behavior; migration is additive.
- Contract tests cover schema, missing preference, invalid timezone, and bypass.
- Feature flag controls UI exposure; server accepts the API before UI rollout.
- Metrics cover save success, delivery by class, bypass, latency, and errors.

### Delivery plan

- M1: schema, API, authorization, and contract fixture work internally.
- M2: notification service honors preferences in shadow comparison mode.
- M3: UI enables internal users to edit settings end to end.
- M4: canary and measured expansion.

Items can proceed in parallel after the contract fixture lands. API owns schema;
notification owner reviews semantics; one integration owner validates the real
boundary.

### Build and review

Each change implements one slice with tests. A fresh review specifically checks
authorization, timezone boundaries, event classification, fallback behavior,
and the risk that visible tests merely mirror the implementation.

### Launch

Internal use runs for one week, followed by 5%, 25%, and 100% cohorts. Expansion
requires save success above target, no rise in missed critical alerts, delivery
latency within SLO, and support volume within guardrail. Disabling the UI flag
and restoring default server behavior are tested rollback actions.

### Learning

At 14 days, product and reliability owners compare outcome and guardrails. The
team either expands the preference model, corrects observed friction, or removes
the feature if its premise failed.

## 21. Adoption plan for an existing organization

### First 30 days: establish the floor

- identify canonical repositories and owners;
- document bootstrap and verify commands;
- protect main and require actionable CI;
- adopt quick/feature/product path selection;
- use `build-change` and independent human review on bounded work;
- inventory secrets, unsupported dependencies, and production rollback gaps.

### Days 31-60: make coordination reliable

- introduce accepted briefs and material design reviews;
- define component and interface ownership;
- standardize small change descriptions and review severity;
- add contract tests at high-conflict boundaries;
- limit work in progress and name integration checkpoints;
- pilot `review-change` in a fresh AI context.

### Days 61-90: close the production loop

- make artifacts immutable and traceable;
- define rollout thresholds and release authority;
- establish SLOs, dashboards, alerts, runbooks, and restore drills;
- measure delivery and product outcomes together;
- evaluate AI impact on quality, rework, cycle time, and developer experience;
- remove workflow steps that do not prevent an identified failure.

Do not roll out every control to every repository simultaneously. Begin with a
representative product, collect evidence, revise the standard, then scale it
through templates and platform automation.

## 22. How to invoke the facade and six lifecycle skills

Use natural language or the skill name. Examples:

```text
Use $specify-project for this greenfield product. Interview me exactly one
question at a time, recommend a default with every question, accept the product
frame before the technical design, and create one canonical SPECIFICATION.md.
Do not create implementation tasks or code.

Use $define-product to turn this idea into an accepted feature brief. Challenge
the problem, success measure, non-goals, and assumptions; do not design it yet.

Use $design-solution for the accepted brief. Inspect the repository first,
compare credible options, and stop for material API, data, or security decisions.

Use $plan-delivery to create outcome milestones and only the next wave of Ready
work for three developers. Name owners, reviewers, and simple dependencies.

Use $build-change for work item PREF-17. Pair with me, inspect before planning,
make one reviewable change, and run the repository validation.

Use $review-change on the exact current diff against the accepted brief and
design. Do not modify files. Report only evidence-based findings.

Use $launch-product to assess this release candidate. Produce readiness evidence,
stages, thresholds, rollback, owners, and unresolved go/no-go decisions. Do not
deploy or expand exposure.
```

The skill guides the conversation. The accepted artifact and current repository
remain the source of truth.

## 23. Common anti-patterns

| Anti-pattern | Consequence | Correction |
|---|---|---|
| full product generated from one prompt | hidden assumptions and unreviewable change | milestone and bounded vertical slices |
| specification, brief, and design repeat the same facts | conflicting sources of truth | choose the combined or modular artifact path |
| design written without repository inspection | imaginary components and commands | verify current system first |
| every change produces every document | process fatigue and stale copies | lightest safe path |
| no brief because requirements are "obvious" | disagreement appears during review | short acceptance and non-goals |
| AI writes and approves its own tests/code | correlated blind spots | fresh review plus human approval |
| months-long task decomposition | stale plan and false certainty | rolling-wave milestones |
| developers each change a shared contract | integration failures | contract owner, fixture, and checkpoint |
| CI has many ignored warnings | false confidence | actionable gates with owners |
| merge treated as success | user harm or unused feature unseen | measured rollout and outcome review |
| permanent flags and dual paths | growing operational complexity | owner, deadline, cleanup item |
| process metric becomes target | gaming and lower quality | balanced product, quality, flow, and human measures |

## 24. Canonical artifact map

| Question | Source of truth |
|---|---|
| Why are we doing this? | accepted combined specification, or product/feature brief |
| What behavior is accepted? | specification or brief acceptance scenarios |
| How and why is the system designed? | accepted specification, or design and decision records |
| What work is active and who owns it? | issue/project tracker |
| How does the software behave? | source and tests |
| What exactly was reviewed? | pull request/change snapshot |
| What can be released? | CI artifact and provenance |
| Is it safe to expand? | launch record plus observability evidence |
| What happened in production? | telemetry, incident record, outcome review |

Git history is the revision record. Use simple document states such as Draft,
Accepted, Active, and Superseded. Version APIs and schemas when consumers require
a compatibility contract; do not invent revision identifiers for ordinary plans.

## 25. Authoritative references

- [Software Engineering at Google](https://abseil.io/resources/swe-book)
- [Google Engineering Practices: Code Review](https://google.github.io/eng-practices/review/)
- [Google Engineering Practices: Small CLs](https://google.github.io/eng-practices/review/developer/small-cls.html)
- [DORA: Trunk-Based Development](https://dora.dev/capabilities/trunk-based-development/)
- [DORA: Working in Small Batches](https://dora.dev/capabilities/working-in-small-batches/)
- [Google SRE Workbook: Canarying Releases](https://sre.google/workbook/canarying-releases/)
- [Google Research: Accelerating Code Migrations with AI](https://research.google/blog/accelerating-code-migrations-with-ai/)
- [Google Research: AI as a Collaborative Partner in Software Engineering](https://research.google/pubs/towards-ai-as-a-collaborative-partner-a-taxonomy-of-ai-agent-behavior-in-software-engineering/)
- [DORA AI Capabilities Model](https://cloud.google.com/blog/products/ai-machine-learning/introducing-doras-inaugural-ai-capabilities-model)
- [SLSA specification](https://slsa.dev/spec/v1.2/)

## 26. Companion guides

- [Google-Inspired Python Project Handbook](PYTHON-PROJECT-HANDBOOK.md)
- [Google-Inspired Language-Agnostic Project Handbook](LANGUAGE-AGNOSTIC-PROJECT-HANDBOOK.md)
- [Product Development Workflow](../for-human/development-guide.md)
- [Four Common Workflow Recipes](../WORKFLOW-COOKBOOK.md)
- [AI Agent Workflow](../for-ai/ai-agent-guidelines.md)

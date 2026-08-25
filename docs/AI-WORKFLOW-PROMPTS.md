# Human-AI Workflow Prompt Templates

Use these templates with the repository's primary `orchestrator` agent. The
orchestrator routes internally to `specify-project`, `define-product`,
`design-solution`, `plan-delivery`, `build-change`, `review-change`,
`pr-review`, `clean-code-review`, `critique-review`, or `launch-product`; developers do not need to invoke a
skill manually.

Every replaceable variable is highlighted as **`{{UPPER_SNAKE_CASE}}`**. Replace
all applicable variables before sending a prompt. Delete an optional line or
write `Not provided` rather than letting the AI invent missing information.

## Choose a template

| Human-facing step | Situation | Template |
|---|---|---|
| Understand | New product or whole-product blueprint | 1 |
| Understand | Bounded feature or product proposal | 2 |
| Understand | Material technical design | 3 |
| Understand | Multi-developer milestones and work items | 4 |
| Build and Review | Ready work item using three agents | 5 |
| Build and Review | Small bug fix | 6 |
| Review | Standalone independent review | 7 |
| Ship | Release readiness and rollout plan | 8 |
| Ship/Understand | Rollout decision or next milestone | 9 |

## 1. Guided greenfield or whole-product specification

Use this when one recommendation-led interview should create a canonical
`SPECIFICATION.md`. It routes to `specify-project` and must not produce an
implementation plan or code.

> I want to define **`{{PROJECT_NAME}}`** from the following idea or blueprint:
>
> **`{{IDEA_OR_BLUEPRINT}}`**
>
> Existing evidence, research, or source material:
> **`{{EVIDENCE_AND_LINKS}}`**
>
> Known constraints or policies:
> **`{{CONSTRAINTS_AND_POLICIES}}`**
>
> Guide me through the greenfield/whole-product specification workflow. Ask
> exactly one targeted question per response and include your recommended
> default. Distinguish verified facts, assumptions, and decisions. Challenge
> unsafe or vague answers with concrete trade-offs.
>
> Obtain my acceptance of the product frame before beginning technical design.
> When every material product and architecture question is resolved, generate
> one canonical `SPECIFICATION.md`. Do not create a roadmap, sprint plan, work
> items, implementation guide, or code.

## 2. Product or feature definition

Use this for a bounded feature or modular product path. It routes to
`define-product` and deliberately avoids technical solution design.

> Help me define **`{{FEATURE_OR_PRODUCT_NAME}}`**.
>
> Current request or problem statement:
> **`{{REQUEST_OR_PROBLEM}}`**
>
> Known users and evidence:
> **`{{USERS_AND_EVIDENCE}}`**
>
> Existing product/specification context:
> **`{{UPSTREAM_CONTEXT_LINKS}}`**
>
> Constraints:
> **`{{CONSTRAINTS}}`**
>
> Clarify the user, problem, outcome, success measures, essential scenarios,
> first-release scope, non-goals, constraints, and evidence-seeking assumptions.
> Recommend the lightest safe workflow. Ask only questions that materially
> change those facts. Do not design the technical solution, assign developers,
> or create implementation tasks. Keep the brief Draft until I explicitly
> accept outcome, scope, non-goals, and success measures.

## 3. Technical solution design

Use when an accepted outcome requires shared architecture, API, data,
reliability, security, migration, or rollout decisions. It routes to
`design-solution`.

> Design the technical solution for **`{{FEATURE_OR_PRODUCT_NAME}}`**.
>
> Accepted brief or specification:
> **`{{ACCEPTED_PRODUCT_AUTHORITY}}`**
>
> Relevant repository area and existing design/API sources:
> **`{{REPOSITORY_AND_DESIGN_LINKS}}`**
>
> Material risks or constraints:
> **`{{RISKS_AND_CONSTRAINTS}}`**
>
> Inspect the current repository, tests, conventions, components, ownership,
> APIs, schemas, failure behavior, and prior decisions before proposing a
> solution. Identify only the shared decisions required for safe implementation.
> For every cross-component contract define its owner, consumers, shape,
> guarantees, errors/timeouts/retries, compatibility expectations, and a
> contract fixture or test where parallel work depends on it.
>
> Recommend one solution and explain meaningful alternatives and trade-offs.
> Ask me only about decisions affecting product behavior, public contracts,
> persistent data, risk, cost, or ownership. Do not prescribe private methods,
> create work items, or assign developers. Mark the design Accepted only after I
> confirm the material decisions.

## 4. Multi-developer delivery planning

Use after the product and required architecture are accepted. It routes to
`plan-delivery` and lets the AI discover relevant subsystem and contract seams.

> Plan delivery for **`{{PROJECT_OR_FEATURE_NAME}}`** with
> **`{{TEAM_SIZE}}`** developers.
>
> Accepted specification/brief/design/API authority:
> **`{{ACCEPTED_AUTHORITY_LINKS}}`**
>
> Team capabilities and availability:
> **`{{TEAM_CAPABILITIES_AND_AVAILABILITY}}`**
>
> Current commitments or fixed dates:
> **`{{COMMITMENTS_OR_DATES}}`**
>
> First inspect the accepted authority and repository. Recommend the next
> demonstrable outcome milestone. Show the subsystem and contract boundaries it
> touches, fixed decisions, blocking unknowns, shared hotspots, and work that can
> proceed concurrently. Return material architecture gaps to technical design
> rather than inventing them.
>
> Plan the current milestone in detail and later milestones coarsely. Split
> current work by independently testable behavior, not merely technical layers.
> For each item provide outcome and acceptance, authority links, recommended
> owner and independent reviewer, risk, validation, status, and only these
> dependency relations: `Blocked by`, `Integrates with`, and `Lands after`.
> Define integration checkpoints and their entry/completion evidence. Default to
> one implementation item per developer and no more than two active reviews per
> reviewer. End with `Ready now`, `Parallel`, `Blocked`, `Integration points`,
> and `Human decisions required`. Do not implement anything.

## 5. Execute one ready work item with orchestrator, builder, and reviewer

Use this for the normal Build phase. The orchestrator creates the plan and
passes all handoffs; the human does not copy messages between subagents.

> Execute work item **`{{WORK_ITEM_ID}}`** for
> **`{{PROJECT_OR_FEATURE_NAME}}`** using the repository's three-agent workflow.
>
> Work-item source:
> **`{{WORK_ITEM_LINK_OR_TEXT}}`**
>
> Accepted specification/brief/design/API authority:
> **`{{AUTHORITY_LINKS}}`**
>
> Integration constraints and dependencies:
> **`{{DEPENDENCIES_AND_CHECKPOINTS}}`**
>
> Required validation or test environment:
> **`{{VALIDATION_REQUIREMENTS}}`**
>
> Act as the primary orchestrator and do not implement product code yourself.
>
> **Plan:** Inspect repository instructions, Git state, relevant code, tests,
> ownership, comparable implementations, and authority. Confirm the item is
> ready. Present the focus card and generate the complete repository-grounded
> implementation plan using the `build-change` template. Identify exact
> expected files/symbols, steps with tests, validation commands, risks, rollout,
> decisions, base commit, allowed scope, and stop conditions. Do not ask me to
> write the plan. Ask me only for a material product, API, data, dependency,
> architecture, security, destructive, scope, ownership, or risk decision.
> Present the plan and obtain my approval before delegation.
>
> **Build:** Invoke the `builder` subagent with the accepted work item and plan,
> exact authority links, base commit, allowed scope, integration constraints,
> validation, and stop conditions. The builder may edit and test but may not
> invoke agents, alter accepted authority, commit, push, merge, publish, deploy,
> migrate data, or expand rollout. Require an evidence receipt with exact base
> and head snapshots.
>
> **Review:** After validating the builder handoff, invoke `reviewer` in a fresh
> task with the exact base-to-head snapshot, work item, accepted plan, authority,
> and evidence receipt. The reviewer must remain read-only and end with `READY
> FOR HUMAN APPROVAL`, `CHANGES REQUIRED`, or `BLOCKED BY MISSING EVIDENCE`.
>
> **Correct:** Route actionable findings back to `builder` without asking me to
> copy them. Reinvoke `reviewer` on the corrected exact snapshot. Stop after two
> correction attempts with the same root cause, when the accepted plan must
> change materially, when work collides, or when safe validation is unavailable.
>
> **Handoff:** Return delivered behavior, changed files/components, exact
> validation actually run, acceptance evidence, deviations, limitations,
> reviewer decision, residual risks, and exact snapshot. Stop before commit or
> merge for my approval.

## 6. Small bug fix with the three-agent workflow

Use this shorter Build prompt for a local, reversible defect with an obvious
accepted outcome. It still uses `build-change` and independent `review-change`.
When a finding needs a concrete patch, use `critique-review` to draft a
suggestion only, then explicitly hand it to `build-change` or ask the developer
to apply it before repeating independent review.

> Fix **`{{BUG_ID_OR_TITLE}}`** using the orchestrator, builder, and reviewer.
>
> Reproduction and expected behavior:
> **`{{REPRODUCTION_AND_EXPECTED_BEHAVIOR}}`**
>
> Relevant issue, specification, or code context:
> **`{{AUTHORITY_OR_CONTEXT}}`**
>
> Non-goals:
> **`{{NON_GOALS}}`**
>
> Inspect and reproduce the defect. Present an inline focus card and recommend
> the smallest correction with targeted validation. If the fix is truly local
> and contains no material decision, ask for permission to delegate without
> manufacturing a separate plan document. Invoke `builder`, then invoke the
> read-only `reviewer` on the exact diff and evidence. Route findings
> automatically for at most two correction rounds. Stop for scope expansion,
> missing authority, contract changes, unsafe validation, or repeated failure.
> Return the final evidence and reviewer decision; stop before commit or merge.

## 7. Standalone independent change review

Use when reviewing an existing pull request, branch, commit, or patch without
running the complete orchestrated Build flow. It routes to `review-change`.

> Independently review **`{{CHANGE_REFERENCE}}`**.
>
> Exact base snapshot: **`{{BASE_COMMIT}}`**
>
> Exact head snapshot: **`{{HEAD_COMMIT}}`**
>
> Work item and acceptance criteria:
> **`{{WORK_ITEM_AND_ACCEPTANCE}}`**
>
> Accepted specification/design/API authority:
> **`{{AUTHORITY_LINKS}}`**
>
> Implementer validation evidence:
> **`{{VALIDATION_EVIDENCE}}`**
>
> Use the independent reviewer in a fresh context. Review only this exact
> base-to-head snapshot and do not modify files. Prioritize correctness,
> security/privacy, permissions, data loss, concurrency, compatibility, error
> behavior, tests, architecture, scope, maintainability, and rollout. For each
> actionable finding provide severity P0-P3, tight location, observed evidence,
> impact, and a testable correction. Do not invent requirements or block on
> personal style. End with `READY FOR HUMAN APPROVAL`, `CHANGES REQUIRED`, or
> `BLOCKED BY MISSING EVIDENCE` and list residual risks.

## 8. Release readiness and rollout planning

Use when a completed feature or product approaches production. It routes to
`launch-product`; it never authorizes deployment by itself.

> Assess release readiness for **`{{RELEASE_NAME_OR_VERSION}}`**.
>
> Accepted outcome and release scope:
> **`{{OUTCOME_AND_SCOPE}}`**
>
> Release candidate artifact/configuration and source revision:
> **`{{ARTIFACT_CONFIG_AND_REVISION}}`**
>
> Test, security, migration, compatibility, and operational evidence:
> **`{{READINESS_EVIDENCE}}`**
>
> Observability and support links:
> **`{{OBSERVABILITY_AND_SUPPORT}}`**
>
> Required approvers and policy:
> **`{{APPROVERS_AND_POLICY}}`**
>
> Prepare an evidence-based readiness assessment and staged rollout plan. Cover
> migrations, feature flags, internal/dogfood testing, immutable artifact
> promotion, stage audiences, minimum evidence windows, success and health
> thresholds, pause/rollback thresholds, monitors, ownership, support, rollback
> readiness, and cleanup. Mark every unverified critical item as a blocker with
> an owner and evidence-producing action. Recommend `expand`, `hold`, `roll
> back`, or `revise`. Do not deploy, publish, migrate, or expose users without my
> explicit authorization.

## 9. Rollout decision, post-launch learning, or next milestone

Use after a rollout stage or milestone demonstration. It closes Ship and feeds
evidence back into Understand and `plan-delivery`.

> Evaluate **`{{MILESTONE_OR_ROLLOUT_STAGE}}`** for
> **`{{PROJECT_OR_RELEASE}}`**.
>
> Accepted outcome, success measures, and guardrails:
> **`{{OUTCOME_MEASURES_AND_GUARDRAILS}}`**
>
> Exact deployed artifact/configuration and exposure:
> **`{{ARTIFACT_AND_EXPOSURE}}`**
>
> Product, reliability, security, cost, and support evidence:
> **`{{OBSERVED_EVIDENCE}}`**
>
> Incidents, changed assumptions, or unresolved risks:
> **`{{INCIDENTS_AND_RISKS}}`**
>
> Compare observed evidence with the accepted decision rules. Distinguish facts,
> inference, and missing evidence. Recommend `expand`, `hold`, `roll back`,
> `revise`, `keep`, or `stop`, with the exact reason and human decision needed.
> If this is a completed development milestone, recommend the next demonstrable
> outcome and update only the next delivery wave after I accept it. Do not
> change exposure or release state without explicit authorization.

## Placeholder checklist

Before sending a prompt:

- replace every **`{{PLACEHOLDER}}`**;
- use repository paths, issue IDs, and commit hashes where available;
- link to accepted authority instead of copying it;
- state `Not provided` for genuinely unknown information;
- do not invent numerical targets, policies, owners, or production evidence;
- keep one work item and one review purpose per Build invocation; and
- start separate branches/worktrees and orchestrator sessions for concurrent
  developers.

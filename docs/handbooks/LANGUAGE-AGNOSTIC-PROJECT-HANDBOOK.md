# Google-Inspired Language-Agnostic Project Handbook

**Audience:** developers, technical leads, engineering managers, platform teams,
and AI-assisted contributors working in any programming language

**Purpose:** provide a precise repository and engineering standard that can be
adapted to a service, library, command-line tool, desktop application, mobile
application, embedded system, data product, or monorepo

**Interpretation of "Google style":** this handbook applies publicly documented
Google engineering principles: one source of truth, clear ownership, design
review for consequential work, small reviewed changes, trunk-oriented
development, automated evidence, and gradual release. It does not claim that
every Google team uses this exact directory tree, Git host, or toolchain.

## 1. The project standard

Use the repository's four human-facing steps—Understand, Build, Review, and
Ship—from the [Product Development Workflow](../for-human/development-guide.md). Developers
describe their work normally; the AI routes to detailed skills internally.

A production repository has a small set of enforceable properties:

1. The problem, users, outcome, scope, and success measures are written down.
2. One repository location is canonical; the default branch is protected.
3. A new developer can build, test, and run the project from documented commands.
4. The build is reproducible from declared tools, dependencies, and lockfiles.
5. Formatting, linting, static analysis, tests, and artifact construction run in CI.
6. Architecture boundaries, public contracts, and ownership are explicit.
7. Changes are small, independently reviewed, and safe to integrate continuously.
8. Secrets are external to source control and automation uses least privilege.
9. Releases are immutable, observable, staged when risk requires it, and reversible.
10. Documentation is owned, reviewed, searchable, and maintained with the code.
11. AI proposes and performs bounded work; humans retain consequential authority.
12. Production evidence closes the loop from delivery back to product decisions.

These are outcomes. Teams may select different tools if they preserve the same
properties.

## 2. Decide before creating files

Record the following in an issue or short product brief before scaffolding a
significant project:

| Decision | Required answer |
|---|---|
| User | Who has the problem? |
| Problem | What costly or frustrating situation exists today? |
| Outcome | What user behavior or system state should change? |
| Evidence | How will the team know the outcome occurred? |
| Product shape | Service, library, application, tool, data job, or collection? |
| Runtime | Where will it execute, and who operates that environment? |
| Data | What is stored, for how long, and under which classification? |
| Interfaces | Which humans and systems consume it? |
| Risk | What happens if it is wrong, unavailable, slow, or compromised? |
| Ownership | Who accepts changes, incidents, and compatibility obligations? |
| Support window | Which operating systems, runtimes, architectures, or browsers? |
| Constraints | Budget, deadline, regulation, latency, scale, accessibility? |

Do not start with an architecture diagram for an unvalidated problem. For a
throwaway experiment, write the hypothesis, time box, evaluation, and deletion
date. Promote an experiment to a product only after replacing shortcuts that are
not safe to operate.

When a new product needs a thorough combined product-and-architecture interview,
use `specify-project` and create one canonical `SPECIFICATION.md` before the
repository skeleton. It accepts the product frame before technical design. For a
bounded feature, use the lighter modular `define-product` and optional
`design-solution` path. Do not duplicate the same facts across a specification,
brief, and design.

## 3. Create the repository

### 3.1 Establish the canonical location

1. Create one repository in the organization's approved version-control system.
2. Set `main` as the default branch.
3. Disable direct pushes to `main` except controlled automation or emergencies.
4. Require successful checks and at least one independent approval.
5. Require review from owners for sensitive paths.
6. Prevent force pushes and branch deletion on protected release branches.
7. Enable secret scanning, dependency alerts, and audit logging where available.
8. Define who may administer repository rules; keep that group small.

A repository is canonical only if issues, source, configuration, reviews, build
definitions, and release provenance point back to it. Do not maintain active
copies in shared drives or chat attachments.

### 3.2 Add the initial files

Use this tree as a starting point, then remove directories that have no current
purpose:

```text
project/
|-- SPECIFICATION.md              # optional canonical greenfield blueprint
|-- .agents/
|   `-- skills/                  # repository-specific AI workflows
|-- .github/                     # or equivalent host configuration
|   |-- CODEOWNERS
|   |-- pull_request_template.md
|   `-- workflows/
|       |-- presubmit.yml
|       `-- release.yml
|-- configs/                     # non-secret configuration defaults/schemas
|-- deploy/                      # deployment definitions and environment policy
|-- docs/
|   |-- product/                 # briefs and outcome decisions
|   |-- design/                  # system designs and durable decisions
|   |   `-- decisions/
|   |-- operations/              # runbooks, SLOs, dashboards, incident guidance
|   |-- features/                # bounded feature briefs/designs when useful
|   |-- for-ai/                  # concise repository instructions for AI
|   |-- for-human/development-guide.md
|   `-- README.md
|-- examples/                    # tested consumer examples
|-- src/                         # product source; adapt to ecosystem convention
|-- tests/
|   |-- unit/
|   |-- contract/
|   |-- integration/
|   |-- end_to_end/
|   `-- performance/
|-- tools/                       # checked-in developer/build helpers
|-- .editorconfig
|-- .gitignore
|-- AGENTS.md                    # short AI entry policy
|-- CHANGELOG.md                 # only if releases need a curated change record
|-- CONTRIBUTING.md
|-- LICENSE
|-- README.md
|-- SECURITY.md
`-- <build and dependency manifests>
```

Empty architecture is not architecture. Do not pre-create `controllers`,
`services`, `repositories`, or dozens of packages merely because a template
contains them. Add a boundary when it isolates a real responsibility, owner,
dependency, or change rate.

### 3.3 Define the purpose of root files

| File | Minimum content |
|---|---|
| `README.md` | purpose, status, quick start, commands, docs links, support |
| `CONTRIBUTING.md` | prerequisites, workflow, quality gates, review and release rules |
| `SECURITY.md` | supported versions, private reporting channel, response expectations |
| `LICENSE` | approved legal terms; do not invent a licence |
| `.gitignore` | generated output, local state, caches, credentials; never source |
| `.editorconfig` | charset, line endings, indentation, trailing whitespace |
| `AGENTS.md` | concise AI constraints and links; no duplicated design specification |
| build manifest | source roots, targets, dependencies, tasks, versions |
| lockfile | exact resolved dependencies; commit when the ecosystem supports it |

The README is the front door, not the complete manual. Link to canonical details
instead of creating a second version of them.

## 4. Choose and pin the toolchain

For each tool category, select one default and write down the supported version:

| Category | Examples | Required property |
|---|---|---|
| Runtime/compiler | JDK, Go, Rust, Node.js, Python, .NET | supported version is explicit |
| Package manager | ecosystem standard | lockfile and authenticated registry |
| Build runner | native tool, Make, task runner, Bazel | one documented entry point |
| Formatter | ecosystem standard | deterministic and automated |
| Linter/static checks | ecosystem standard | local and CI parity |
| Type/API checker | compiler, type checker, API diff | breaks unsafe contracts early |
| Test runner | ecosystem standard | machine-readable reports and filtering |
| Artifact builder | compiler/packager/container builder | immutable, reproducible output |
| Vulnerability checks | dependency and source scanners | actionable severity policy |

Pin tool versions in files the repository can review. Update them through small,
automated pull requests with the full checks. Avoid instructions that merely say
"install the latest version" because two developers then use different systems.

## 5. Build and developer experience

### 5.1 Offer one obvious command surface

Every repository should expose equivalent tasks, regardless of implementation:

```text
bootstrap   install or verify tools and dependencies
format      rewrite source into canonical formatting
lint        run non-mutating style and policy checks
typecheck   compile or check static types and APIs
test        run the normal deterministic test suite
test-all    run slower integration/end-to-end checks
build       create the releasable artifact
run         start the project locally with safe defaults
verify      run the complete presubmit gate
clean       remove only documented generated output
```

The task runner is an adapter, not a second build system. It should invoke the
same compiler and package-manager commands developers can diagnose directly.
Commands must be non-interactive in CI and return nonzero on failure.

### 5.2 Make builds reproducible

- Declare every input: source, tool version, dependency, build option, and
  generated file source.
- Resolve dependencies from lockfiles or content-addressed manifests.
- Do not depend on undeclared files from a developer's home directory.
- Normalize timestamps and ordering where the artifact format permits it.
- Separate platform-specific targets instead of relying on accidental host state.
- Build release candidates in hosted automation from a known, green commit.
- Record source commit, toolchain, dependency set, and artifact digest.
- Test the artifact that will be released, not a separately rebuilt approximation.

Hermeticity is a spectrum. Document unavoidable external inputs and verify them
with checksums, versions, and access controls.

### 5.3 Treat generated code as a product of its generator

Check in generated output only when consumers or build constraints require it.
In either case:

1. pin the generator;
2. store the source definition;
3. provide one regeneration command;
4. fail CI if regeneration changes a supposedly current file; and
5. review semantic source changes, not just generated noise.

## 6. Dependencies and supply chain

For every new dependency, record or review:

- the exact capability it supplies;
- why existing platform or standard-library functionality is insufficient;
- licence and organizational policy compatibility;
- maintenance activity and security posture;
- transitive dependency cost;
- runtime, binary-size, privacy, and availability effects;
- ownership and replacement plan.

Separate production, development, test, build, and optional dependencies. Remove
unused dependencies. Authenticate private registries. Use short-lived credentials
in automation. Verify checksums or signatures when supported.

For higher-assurance projects, generate a software bill of materials, sign
artifacts, retain provenance, and raise the target SLSA level deliberately.

## 7. Architecture and source organization

### 7.1 Organize around responsibilities

A healthy component has:

- one clear purpose;
- a small public interface;
- private implementation details;
- explicit incoming and outgoing dependencies;
- an owner for changes and incidents; and
- tests at the boundary where failures matter.

Prefer dependency direction from volatile delivery mechanisms toward stable
domain rules, or another explicit architecture appropriate to the project.
Business logic should not require a live network, UI framework, or production
database merely to be tested.

### 7.2 Make contracts explicit

A contract can be a typed interface, schema, protocol, command syntax, database
migration rule, event definition, or user-facing behavior. For each shared
contract specify:

- producer and consumers;
- syntax and semantics;
- validation and error behavior;
- compatibility rules;
- versioning and deprecation window;
- security and privacy classification;
- performance or availability expectations; and
- contract tests or fixtures.

Parallel development is safe only when collaborators can build against an
accepted contract and independently verify compatibility.

### 7.3 Record durable decisions

Use a short design decision record when a choice will otherwise be repeatedly
re-litigated. Include context, decision, considered alternatives, consequences,
owner, date, and superseding decision. Do not create a record for routine local
implementation choices that are obvious in code.

## 8. Configuration, environments, and secrets

Define a typed or schema-validated configuration interface. Safe local defaults
may live in Git. Secrets never do.

- Keep development, test, staging, and production differences in deployment
  configuration, not conditional source branches.
- Retrieve secrets from an approved secret manager at runtime.
- Use workload identity or short-lived tokens instead of static cloud keys.
- Validate required values at startup with clear, non-secret error messages.
- Define precedence among command arguments, environment, files, and remote config.
- Rotate credentials and test the rotation procedure.
- Redact tokens, credentials, personal data, and sensitive payloads from logs.

An environment variable is a transport mechanism, not automatically a secure
secret-management system.

## 9. Code conventions and maintainability

Adopt the ecosystem's established style guide and enforce the machine-checkable
parts. Repository rules should cover:

- names, file layout, imports, and visibility;
- error handling and resource cleanup;
- concurrency and cancellation;
- logging and telemetry;
- nullability or optional data;
- API documentation;
- generated code;
- dependency injection and test seams; and
- forbidden unsafe constructs.

Optimize for the reader. Prefer straightforward code, precise names, small
interfaces, and comments that explain why. Avoid framework layers, generalized
factories, or configuration points without a demonstrated second use.

Warnings must either fail the relevant gate, be suppressed with an owned reason,
or be removed. A permanently noisy check is not a control.

## 10. Testing and evaluation

### 10.1 Use a layered test strategy

| Layer | Purpose | Normal trigger |
|---|---|---|
| Unit | focused deterministic behavior in one component | risk-based; use when it gives useful confidence |
| Contract | compatibility between independently changed components | interface changes and presubmit |
| Integration | real adapters such as database, queue, filesystem, or service | primary confidence at important boundaries; presubmit or postsubmit |
| End-to-end | critical user journey across deployed boundaries | staging and selected presubmit |
| Performance | latency, throughput, memory, scale, regression | scheduled and release candidates |
| Resilience | timeout, retry, partial failure, restore, failover | risk-based and scheduled |
| Security | abuse cases, authorization, dependency and source scanning | every relevant change plus scheduled |
| Accessibility/usability | actual user interaction quality | feature and release review |

Keep the suite intentionally small: cover the main workflows, important failure
paths, and selected rare edge cases whose impact justifies their cost. Prefer
integration tests that exercise real boundaries when they provide stronger
confidence than isolated unit tests. Test observable behavior, not private
method shape, and do not use coverage percentages as a quality target.

### 10.2 Protect the evaluator

AI-assisted implementation can optimize toward visible tests. For critical
behavior:

- derive acceptance tests from the brief and contracts, not from generated code;
- have a human or independent reviewer examine test adequacy;
- use hidden, immutable, or independently owned evaluation cases where gaming is
  consequential;
- add property, fuzz, mutation, differential, or fault-injection tests only when
  the risk justifies their cost;
- validate negative paths and authorization boundaries;
- track false positives and false negatives in quality gates.

A passing test suite proves only what the suite measures.

### 10.3 Control flaky tests

Do not normalize retries as success. On a flaky failure:

1. preserve diagnostics;
2. identify an owner;
3. reproduce or quantify the failure rate;
4. fix it promptly, or quarantine it with a deadline and visible risk; and
5. restore the protection or remove the invalid test.

## 11. Version control and change management

### 11.1 Use trunk-oriented development

- Branch from current `main`.
- Keep branches short-lived.
- Rebase or merge the latest target branch before final validation according to
  repository policy.
- Integrate incomplete but safe work behind an inactive feature flag.
- Delete merged branches.
- Release from known green commits rather than long-lived integration branches.

Long-lived branches hide integration risk. Use them only for an explicit support
or release policy, with a named merge strategy.

### 11.2 Keep changes small

One change should have one coherent purpose. Include tests with the behavior.
Separate preparatory refactors, mechanical migration, generated output, and
behavior changes when they can be reviewed independently.

Line count is a warning, not a universal rule. Reviewability is the requirement:
the reviewer must be able to understand the complete effect, validate it, and
spot unrelated work. Google guidance notes that roughly 100 lines can often be
reasonable and 1,000 lines usually is not, while generated code and deletions
need judgment.

### 11.3 Require a useful change description

Every pull request or change list should state:

- problem and intended outcome;
- what changed and why this approach was chosen;
- what deliberately did not change;
- test and validation evidence;
- risk, compatibility, migration, security, and privacy effects;
- screenshots or recordings for user-interface changes;
- rollout and rollback plan when behavior reaches users; and
- links to the accepted brief, design, decision, and work item.

Commit messages and review descriptions explain intent and rationale; the diff
already shows mechanics.

## 12. Code review

The author and approving reviewer must differ. Review the exact snapshot that
will merge. The reviewer examines:

1. **Design:** does the change belong here and preserve system boundaries?
2. **Functionality:** does behavior meet the accepted outcome, including errors?
3. **Complexity:** is the solution simpler than its problem warrants?
4. **Tests:** would they fail for realistic regressions, and are key gaps present?
5. **Security/privacy:** are identity, authorization, validation, data, and logs safe?
6. **Concurrency/resources:** are cancellation, cleanup, ordering, and limits correct?
7. **Compatibility:** are APIs, schemas, migrations, and clients protected?
8. **Operations:** are metrics, diagnostics, alerts, rollout, and rollback adequate?
9. **Maintainability:** can the owning team understand and modify it later?
10. **Scope:** is unrelated change absent?

Review aims to improve overall code health, not demand theoretical perfection.
Block correctness and material maintainability problems. Label optional ideas as
non-blocking. Resolve disagreement with evidence, the design owner, and the
repository's escalation route.

## 13. Continuous integration

### 13.1 Presubmit pipeline

Order fast, broadly diagnostic checks first:

```text
checkout pinned source
  -> verify toolchain and lockfiles
  -> formatting check
  -> lint/static/type/API checks
  -> unit and contract tests
  -> integration tests
  -> build releasable artifact
  -> security/licence/policy scans
  -> publish reports and artifact metadata
```

For every required job:

- use a pinned, trusted runner image and action/plugin version;
- grant the minimum token permissions;
- avoid executing untrusted contribution code with production credentials;
- cache only content identified by dependency and tool versions;
- set timeouts;
- retain useful logs and test reports;
- make failures actionable and assign an owning team; and
- cancel superseded runs when doing so is safe.

### 13.2 Postsubmit and scheduled checks

Run expensive platform matrices, endurance tests, fuzzing, full vulnerability
scans, restore drills, and production-like integration after merge or on a
schedule. Postsubmit must not become a dumping ground for checks that should have
blocked an unsafe change.

Keep the head of `main` green. If it breaks, revert or fix it immediately before
building more work on an unknown base.

## 14. Security and privacy by design

Threat-model features that cross trust boundaries, accept untrusted content,
change authorization, handle sensitive data, or execute generated instructions.
At minimum:

- authenticate the caller and authorize the exact action;
- validate input at trust boundaries and encode output for its destination;
- restrict network, filesystem, process, and cloud permissions;
- encrypt sensitive data in transit and at rest;
- minimize collection and retention;
- audit consequential actions without logging secrets;
- rate-limit and bound expensive work;
- patch dependencies and supported release lines;
- define private vulnerability reporting and incident ownership; and
- test backup restoration, not only backup creation.

For AI features, treat model output, retrieved documents, web pages, issue text,
and code comments as untrusted data. Tool permissions must be narrower than the
model's possible suggestions. Require human authorization for irreversible or
externally consequential actions.

## 15. Release and operations

### 15.1 Build once, promote the same artifact

Produce an immutable artifact from a green commit. Identify it by digest, attach
provenance and test evidence, deploy it to preproduction, and promote that same
artifact. Do not rebuild separately for production.

### 15.2 Use progressive delivery

Select controls proportional to impact:

1. development and automated evaluation;
2. internal or test-account exposure;
3. staging or production shadow traffic;
4. small canary cohort;
5. measured expansion;
6. broad availability; and
7. cleanup of temporary flags and compatibility code.

Before each step define success metrics, guardrails, observation duration,
decision owner, stop threshold, and rollback action. A feature flag is temporary
control state: name an owner and expiration date.

### 15.3 Make systems operable

Before production, define:

- service-level indicators and objectives where relevant;
- logs, metrics, traces, dashboards, and actionable alerts;
- on-call or support ownership;
- dependency failure and capacity behavior;
- data migration, backup, restore, and disaster-recovery procedures;
- incident response and communication routes;
- rollback or forward-fix procedures; and
- lifecycle, deprecation, and deletion policy.

An alert should represent user impact or an actionable precursor, not merely a
metric crossing an arbitrary line.

## 16. Documentation as engineering work

Documentation should be:

- **canonical:** one authoritative location;
- **owned:** a person or team is accountable;
- **reviewed:** technical changes receive the same scrutiny as code;
- **versioned:** it evolves with the system;
- **discoverable:** readers can find it from the repository front door; and
- **maintained:** obsolete guidance is removed or explicitly superseded.

Use the lightest artifact that preserves a necessary fact:

| Fact | Canonical location |
|---|---|
| problem, outcome, scope, success | brief, or accepted combined specification |
| architecture, contracts, trade-offs | design/API source, or accepted combined specification |
| durable decision rationale | decision record |
| owner and current status | issue/project tracker |
| executable behavior | code and tests |
| developer commands | repository README/contributing guide |
| production response | runbook and observability system |
| rollout state and evidence | launch record |

Avoid status duplicated in multiple Markdown plans. Git is the revision history;
the issue tracker is the current work state.

## 17. Human-AI development policy

This repository's overhauled system uses six lifecycle skills plus one optional
guided project facade:

| Need | Skill | Output or effect |
|---|---|---|
| frame and design a whole new product through one interview | `specify-project` | accepted canonical `SPECIFICATION.md` |
| clarify an idea | `define-product` | accepted product or feature brief |
| decide a material solution | `design-solution` | practical design and decisions |
| coordinate a team | `plan-delivery` | milestones, ready items, owners, dependencies |
| implement one item | `build-change` | small validated change with checkpoints |
| independently inspect it | `review-change` | evidence-based findings on an exact snapshot |
| review a GitHub PR before merge | `pr-review` | exact-head findings and validated inline comments |
| scan coding practices | `clean-code-review` | cataloged Clean Code, GoF, and Python smell findings |
| prepare a suggested correction | `critique-review` | precise read-only diffs from concrete findings |
| release safely | `launch-product` | readiness, rollout, rollback, and learning |

Use the lightest safe path:

- **Quick change:** issue -> `build-change` -> human review -> merge.
- **Suggested correction:** `review-change` -> optional `critique-review` -> explicit handoff to `build-change` -> fresh review.
- **Feature:** `define-product` -> optional `design-solution` -> repeated
  `build-change`/`review-change` -> proportionate rollout.
- **Greenfield guided product:** `specify-project` -> `plan-delivery` -> repeated
  build/review loops -> `launch-product`.
- **Product, modular:** `define-product` -> `design-solution` -> all later
  lifecycle skills, with only the next milestone decomposed in detail.

The guided facade is not a seventh lifecycle stage. It combines the first two
forms of thinking while preserving separate human acceptance for the product
frame and technical design. Once accepted, the specification replaces the brief
and design as their source of truth for that project.

Humans own intent, accepted trade-offs, merge, deployment, migration,
publication, and rollout expansion. AI may inspect, propose, implement, test,
summarize, and independently review within granted scope. AI must stop when a
material choice is absent, requirements conflict, the base changes, work
collides, or required evidence cannot be obtained.

Repository instructions for AI should be concise and hierarchical:

1. organization policy;
2. repository `AGENTS.md`;
3. relevant workflow skill;
4. accepted specification, or accepted brief and design;
5. current work item and exact code snapshot.

Do not paste the entire company handbook into every prompt. Give AI the smallest
complete context, explicitly identify untrusted text, and require citations to
repository facts for consequential claims.

## 18. Solo and team operation

### 18.1 Solo developer

A solo developer still separates roles in time:

- write acceptance behavior before implementation;
- use a fresh AI context for independent review;
- leave high-risk work overnight before final review when feasible;
- protect `main` and require CI even if approval rules cannot require a second human;
- ask a human specialist to review security, privacy, legal, or irreversible data work;
- stage releases and observe evidence before expansion.

Do not manufacture project-management ceremony for a one-day change. Preserve
the decisions and evidence that a future maintainer needs.

### 18.2 Multi-developer team

- Assign each important component and interface an owner.
- Agree shared contracts and fixtures before parallel implementation.
- Default to one implementation item per developer.
- Name the reviewer before work starts; author and reviewer differ.
- Record only `Blocked by`, `Integrates with`, and `Lands after` unless a more
  complex relation genuinely changes scheduling.
- Name an integration owner where several work items meet.
- Demonstrate working behavior at milestone boundaries.
- Plan the next wave from current evidence, not a frozen months-long task list.

Use review capacity, not developer count, as the practical limit on parallel work.

## 19. Project readiness levels

### Level 0: exploration

- hypothesis and time box;
- isolated, non-production data;
- no unsupported security or reliability claims;
- explicit keep/rewrite/delete decision.

### Level 1: collaborative development

- canonical repository and ownership;
- bootstrap, format, lint, test, build, verify commands;
- protected main and review;
- brief and design proportional to risk;
- dependency lock and secrets policy.

### Level 2: releasable

- immutable artifact from hosted CI;
- complete functional, security, compatibility, and migration evidence;
- release notes and version policy;
- staging or equivalent verification;
- rollout and rollback plan.

### Level 3: production operated

- SLOs or explicit operational targets;
- observability and actionable alerts;
- support and incident ownership;
- tested restore and rollback;
- vulnerability and dependency maintenance;
- post-launch measurement and learning.

Do not label a prototype production-ready because its code is tidy.

## 20. Repository inception checklist

### Before the first implementation

- [ ] Brief names user, problem, outcome, success, scope, non-goals, and owner.
- [ ] Runtime, support window, data class, risk, and constraints are explicit.
- [ ] Repository is canonical and `main` is protected.
- [ ] Licence and security reporting policy are approved.
- [ ] Toolchain and dependency versions are pinned.
- [ ] Bootstrap and verify work in a disposable environment.
- [ ] Source and test boundaries follow ecosystem convention.
- [ ] Secrets and environment configuration are externalized.
- [ ] CI runs on the initial change.

### Before accepting a change

- [ ] Acceptance behavior and non-goals are still correct.
- [ ] The diff has one coherent purpose.
- [ ] Contracts, compatibility, data, security, and operations were considered.
- [ ] Tests are behavior-focused and sufficiently independent.
- [ ] All required automation passed on the exact snapshot.
- [ ] An independent human reviewed the complete diff.
- [ ] Documentation, rollout, rollback, and metrics are updated as needed.

### Before production

- [ ] Release artifact is immutable, identifiable, and reproducible.
- [ ] Required security/privacy/compliance reviews are recorded.
- [ ] Migration and rollback were rehearsed at the appropriate fidelity.
- [ ] Dashboards, alerts, runbook, ownership, and support route exist.
- [ ] Exposure stages, thresholds, observation windows, and decision owner exist.
- [ ] Temporary flags and compatibility layers have cleanup owners and dates.

## 21. Common failure modes

| Failure | Better control |
|---|---|
| elaborate tree with empty abstractions | add boundaries only for real responsibilities |
| local-only setup knowledge | executable bootstrap plus concise contributing guide |
| unpinned tools and floating dependencies | reviewed version files and lockfiles |
| long-lived feature branches | small trunk-oriented changes behind safe flags |
| tests written only to satisfy generated code | independent behavior/evaluator review |
| AI infers a missing product or API decision | explicit stop and human decision checkpoint |
| dozens of workflow documents | one source of truth for each kind of fact |
| green CI with unusable errors | actionable logs, owner, and fast feedback |
| production rebuilt from source | promote the same verified artifact |
| feature flag becomes permanent architecture | owner, expiry, cleanup work item |
| rollout judged by absence of complaints | defined product and reliability evidence |
| process copied uniformly to every change | lightest safe path, risk-based controls |

## 22. Authoritative references

- [Software Engineering at Google](https://abseil.io/resources/swe-book)
- [Google Engineering Practices: Code Review](https://google.github.io/eng-practices/review/)
- [Google Engineering Practices: Small CLs](https://google.github.io/eng-practices/review/developer/small-cls.html)
- [DORA: Trunk-Based Development](https://dora.dev/capabilities/trunk-based-development/)
- [DORA: Working in Small Batches](https://dora.dev/capabilities/working-in-small-batches/)
- [Google SRE Workbook: Canarying Releases](https://sre.google/workbook/canarying-releases/)
- [SLSA specification](https://slsa.dev/spec/v1.2/)
- [OpenSSF Scorecard](https://www.scorecard.dev/)
- [GitHub Actions: Secure Use](https://docs.github.com/en/actions/reference/security/secure-use)

## 23. Companion guides

- [Google-Inspired Python Project Handbook](PYTHON-PROJECT-HANDBOOK.md)
- [Idea-to-Production Handbook](IDEA-TO-PRODUCTION-HANDBOOK.md)
- [Product Development Workflow](../for-human/development-guide.md)
- [Four Common Workflow Recipes](../WORKFLOW-COOKBOOK.md)
- [AI Agent Workflow](../for-ai/ai-agent-guidelines.md)

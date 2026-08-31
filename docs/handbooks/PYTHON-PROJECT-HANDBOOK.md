# Google-Inspired Python Project Handbook

**Audience:** first-time contributors, experienced Python developers, technical
leads, and engineering managers

**Purpose:** create a maintainable, testable, secure Python repository from an
empty directory and operate it with disciplined human and AI collaboration

**Interpretation of “Google style”:** this handbook applies Google's publicly
documented engineering principles and Python style rules. The concrete open
source stack—`pyproject.toml`, `src/`, uv, Ruff, mypy, pytest, and GitHub
Actions—is a pragmatic industry implementation. It is not a claim that all
Google teams use these exact tools or repository layouts.

## 1. The standard in one page

Use the repository's four human-facing steps—Understand, Build, Review, and
Ship—from the [Product Development Workflow](../for-human/development-guide.md). Developers
describe their work normally; the AI routes to detailed skills internally.

A production Python project should have:

- one canonical Git repository and a protected `main` branch;
- a written user or business outcome before significant implementation;
- a reviewed technical design for material architecture, API, data, security,
  privacy, or migration decisions;
- a `src/` package layout and a standards-based `pyproject.toml`;
- one dependency declaration, one committed lockfile for applications, and a
  reproducible environment;
- automated formatting, linting, type checking, tests, packaging, and security
  checks available through documented commands;
- tests in the same change as behavior;
- small, focused pull requests reviewed by someone other than the author;
- code ownership, explicit risk escalation, and no secrets in Git;
- automated release artifacts built from reviewed commits;
- observability, progressive rollout, and rollback for production services; and
- repository-local instructions that tell AI tools what they may read, change,
  validate, and escalate.

Google's public engineering guidance emphasizes canonical documentation under
source control, design review for major projects, trunk-oriented development,
small reviewed changes, tests with changes, actionable CI, and progressive
delivery. See the [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html),
[Software Engineering at Google](https://abseil.io/resources/swe-book), and
[Google Engineering Practices](https://google.github.io/eng-practices/review/).

## 2. Decide what you are creating

Do not initialize tooling until you can answer these questions.

For a greenfield product where those answers and the architecture are still
unclear, invoke `specify-project` before scaffolding. It conducts a one-question-
at-a-time interview, accepts the product frame before the technical design, and
creates one canonical `SPECIFICATION.md`. Use the normal `define-product` and
`design-solution` path for a bounded feature or when separate product and design
ownership is more useful. Never create all three artifacts with duplicated facts.

### 2.1 Project type

Choose one primary type:

| Type | Deliverable | Typical entry point | Dependency policy |
|---|---|---|---|
| Library | Wheel/sdist consumed by other projects | Imported public API | Broad compatible runtime ranges; test lowest and highest supported versions |
| CLI application | Installed executable | `[project.scripts]` | Lock exact deployment dependencies |
| Service | Container or deployment artifact | Server process | Lock exact dependencies and configuration schema |
| Batch/worker | Scheduled or queued process | Job command | Lock exact dependencies; make retries/idempotency explicit |
| Research prototype | Evidence, not production | Notebook/script | Time-box it; promote successful logic into a package before production |

Avoid combining unrelated deployables in one package. A monorepo can contain
multiple packages, but each deployable still needs an owner, dependency boundary,
build target, tests, and release unit.

### 2.2 Naming

Define three related names deliberately:

- **Repository:** `example-service`
- **Distribution package:** `example-service`
- **Python import package:** `example_service`

Use lowercase import modules with underscores and no dashes. Google’s Python
guide requires `.py` filenames without dashes and recommends `CapWords` for
classes and `lower_with_under` for modules and functions.

### 2.3 Runtime and support window

Write down:

- minimum and maximum supported Python versions;
- operating systems and CPU architectures;
- whether alternative interpreters are supported;
- expected project lifetime;
- availability, latency, throughput, and recovery objectives if it is a service;
- data classification and regulatory scope; and
- package or deployment destination.

For a new internal application, a reasonable default is the current stable
Python series used by your organization, with one previous series supported only
when consumers require it. This handbook uses Python 3.12 as an example baseline;
replace it with your documented support decision.

### 2.4 Ownership and risk

Before the first feature, name:

- accountable product owner;
- technical owner;
- code reviewers;
- production/on-call owner when applicable;
- security/privacy/compliance contacts; and
- release authority.

For a solo project, one person may hold several roles, but an independent human
review is still required before consequential releases whenever feasible.

## 3. Install the bootstrap tools

The recommended baseline is Git plus uv. uv manages Python versions,
environments, dependencies, lockfiles, commands, and builds. A standards-only
fallback using `python -m venv` and `pip` is acceptable if your organization
does not approve uv; keep the same repository structure and quality gates.

### 3.1 Install Git

Install Git through your operating system or company-managed developer image.
Configure your real identity:

```bash
git config --global user.name "Your Name"
git config --global user.email "you@example.com"
```

Use signed commits or organization-managed signing when policy requires it.

### 3.2 Install uv

Use an organization-approved package manager or a pinned installer version.
Examples:

```bash
# macOS with Homebrew
brew install uv

# Windows with WinGet
winget install --id=astral-sh.uv -e

# Isolated Python installation
pipx install uv
```

Do not execute an unreviewed network installer in a high-assurance environment.
The [official uv installation guide](https://docs.astral.sh/uv/getting-started/installation/)
documents installers, checksums, package managers, and version pinning.

Confirm:

```bash
git --version
uv --version
```

## 4. Create the repository step by step

The following example creates a package named `example_service`.

```bash
uv init --lib example-service
cd example-service
git init -b main
uv python pin 3.12
uv add --dev pytest pytest-cov ruff mypy build
uv lock
uv sync
```

Then create the policy and documentation directories:

```text
.agents/skills/
.github/workflows/
docs/design/decisions/
docs/features/
docs/operations/runbooks/
docs/product/
src/example_service/
tests/unit/
tests/integration/
tools/
```

Use repository-native file operations rather than copying shell commands blindly.
Do not create empty directories merely to resemble the example; add a directory
when it has an owner and purpose.

Copy or adapt the human–AI workflow from this repository:

- [`AGENTS.md`](../../AGENTS.md)
- [`docs/for-human/development-guide.md`](../for-human/development-guide.md)
- [`docs/for-ai/ai-agent-guidelines.md`](../for-ai/ai-agent-guidelines.md)
- [the lifecycle skills and guided project facade](../../.agents/skills/)

Run the workflow validator after copying:

```bash
python scripts/validate-development-workflow.py
```

## 5. Canonical repository layout

Use this as a mature target, not a requirement to create every path on day one.

```text
example-service/
├── SPECIFICATION.md                  # Optional canonical greenfield blueprint
├── .agents/
│   └── skills/                       # Repository-local AI lifecycle skills
├── .github/
│   ├── CODEOWNERS                    # Review ownership
│   ├── PULL_REQUEST_TEMPLATE.md      # Change intent and evidence
│   └── workflows/
│       ├── ci.yml                    # Presubmit quality gates
│       └── release.yml               # Artifact creation/publishing
├── docs/
│   ├── for-human/development-guide.md
│   ├── for-ai/ai-agent-guidelines.md
│   ├── product/                      # Accepted product/feature briefs
│   ├── features/<feature>/           # Feature-local brief/design if useful
│   ├── design/
│   │   ├── system-design.md
│   │   └── decisions/                # Durable ADR-style decisions
│   └── operations/
│       ├── runbooks/
│       └── service-level-objectives.md
├── src/
│   └── example_service/
│       ├── __init__.py
│       ├── __main__.py               # Optional `python -m` entry point
│       ├── cli.py                    # Thin CLI adapter
│       ├── config.py                 # Typed configuration loading
│       ├── domain/                   # Business rules; no infrastructure imports
│       ├── application/              # Use cases and orchestration
│       ├── adapters/                 # Database, HTTP, queues, filesystem
│       └── observability.py          # Logging/metrics/tracing setup
├── tests/
│   ├── unit/                         # Fast, hermetic behavior tests
│   ├── integration/                  # Real component boundaries
│   ├── contract/                     # Producer/consumer compatibility
│   └── end_to_end/                   # Few critical user journeys
├── tools/                            # Reviewed developer/build utilities
├── scripts/
│   └── validate-development-workflow.py
├── .editorconfig
├── .gitignore
├── AGENTS.md                         # Repository AI policy
├── CHANGELOG.md                      # If externally versioned
├── CONTRIBUTING.md
├── LICENSE
├── README.md
├── SECURITY.md
├── pyproject.toml
├── uv.lock                           # Commit for applications and reproducible CI
└── .python-version
```

### Populate the repository contract

Add a conservative `.gitignore`; extend it for your editor and deployment tools
without ignoring source or lockfiles:

```gitignore
.venv/
__pycache__/
*.py[cod]
.pytest_cache/
.mypy_cache/
.ruff_cache/
.coverage
htmlcov/
build/
dist/
*.egg-info/
.env
.env.*
!.env.example
```

Normalize text before the first collaborative change:

```ini
# .editorconfig
root = true

[*]
charset = utf-8
end_of_line = lf
insert_final_newline = true
trim_trailing_whitespace = true

[*.py]
indent_style = space
indent_size = 4

[*.md]
trim_trailing_whitespace = false
```

Create `.github/CODEOWNERS` with real teams or maintainers. Protect workflow,
security, deployment, dependency, schema, and migration paths explicitly:

```text
*                              @example/python-maintainers
/.github/workflows/            @example/platform-security
/deploy/                       @example/platform-security
/src/example_service/security/ @example/security-reviewers
/src/example_service/schema/   @example/data-owners
```

Create a pull-request template that asks for evidence rather than ceremony:

```markdown
## Outcome

<!-- What accepted behavior does this change deliver? -->

## Scope and non-goals

## Design and work-item links

## Validation

<!-- Exact commands, results, screenshots, traces, or benchmarks. -->

## Risk and compatibility

<!-- Security, privacy, data, API, migration, operations. Write "None" with reason. -->

## Rollout and rollback
```

The first `README.md` should contain status, purpose, supported Python versions,
five-minute quick start, canonical `format`/`lint`/`typecheck`/`test`/`build`/`run`
commands, configuration link, architecture link, contribution link, security
reporting link, ownership, and support route. `CONTRIBUTING.md` expands tool
installation, branch/review policy, quality gates, dependency rules, and release
process. `SECURITY.md` states supported versions and an approved private reporting
channel; never ask reporters to disclose an unpatched vulnerability publicly.

### Why use `src/`

The Python Packaging User Guide explains that `src/` prevents the repository
root from accidentally becoming the import source and helps tests exercise the
installed package rather than a convenient but incomplete checkout. It also
prevents packaging mistakes where undeclared files import locally but disappear
from the wheel. See [src layout vs flat layout](https://packaging.python.org/en/latest/discussions/src-layout-vs-flat-layout/).

## 6. Configure `pyproject.toml`

Use `pyproject.toml` as the canonical declaration for build metadata and Python
tools. Do not maintain competing settings in `setup.py`, `setup.cfg`, `tox.ini`,
and several requirements files unless a tool truly requires them.

The following is a strong starting point. Replace names, descriptions, licence,
Python support and entry points intentionally.

```toml
[build-system]
requires = ["hatchling"]
build-backend = "hatchling.build"

[project]
name = "example-service"
version = "0.1.1"
description = "A concise description of the delivered capability."
readme = "README.md"
requires-python = ">=3.12"
license = "Apache-2.0"
authors = [
  { name = "Example Team", email = "team@example.com" },
]
dependencies = []

[project.scripts]
example-service = "example_service.cli:main"

[dependency-groups]
dev = [
  "build",
  "mypy",
  "pytest",
  "ruff",
]

[tool.hatch.build.targets.wheel]
packages = ["src/example_service"]

[tool.ruff]
target-version = "py312"
line-length = 80
src = ["src", "tests"]

[tool.ruff.lint]
select = [
  "B",    # likely bugs
  "C4",   # comprehensions
  "E",    # pycodestyle errors
  "F",    # Pyflakes
  "I",    # import ordering
  "PIE",  # miscellaneous improvements
  "RUF",  # Ruff-specific correctness
  "SIM",  # simplification
  "UP",   # modern Python syntax
]

[tool.ruff.lint.per-file-ignores]
"tests/**/*.py" = ["S101"]

[tool.mypy]
python_version = "3.12"
strict = true
warn_unreachable = true
pretty = true
files = ["src", "tests"]

[tool.pytest.ini_options]
addopts = [
  "-ra",
  "--strict-config",
  "--strict-markers",
  "--import-mode=importlib",
]
testpaths = ["tests"]
xfail_strict = true

```

Notes:

- `uv add` should normally edit dependency declarations; do not hand-edit the
  lockfile.
- A library should declare the broadest honest compatible runtime ranges and
  test every supported Python version. An application should deploy from a
  committed exact lock.
- Coverage reports may be useful diagnostics, but coverage percentages are not
  a quality target or merge gate. Choose tests for realistic behavior,
  important boundaries, and risk.
- If strict typing cannot be enabled immediately, configure narrow per-module
  exceptions with owner and removal issue. Do not disable checking globally.
- Do not use `lint.select = ["ALL"]` without reviewing upgrades; new Ruff rules
  would silently become policy.

The Python Packaging User Guide defines `pyproject.toml` and the standard
`[project]` metadata. See [Writing your pyproject.toml](https://packaging.python.org/en/latest/guides/writing-pyproject-toml/).

## 7. Dependency policy

### 7.1 Add dependencies intentionally

```bash
uv add httpx
uv add --dev hypothesis
uv remove httpx
```

Before adding a package, check:

- whether the standard library or existing dependency already solves the need;
- maintenance activity and responsible disclosure process;
- licence compatibility;
- known vulnerabilities and malicious-package risk;
- transitive dependency size;
- platform and Python support;
- type information;
- expected runtime, memory, and import cost; and
- whether the package becomes part of a public API.

Record the reason for large, security-sensitive, or hard-to-replace dependencies
in the design document or a durable decision.

### 7.2 Lock and reproduce

```bash
uv lock
uv sync
uv run --locked pytest
```

Commit `uv.lock` for applications. For published libraries, commit it for
development/CI reproducibility but test declared dependency ranges rather than
claiming consumers receive your lockfile. CI should fail when the lock is stale;
it should not silently update dependencies.

Upgrade one dependency at a time where practical:

```bash
uv lock --upgrade-package httpx
uv run --locked pytest
```

Review lockfile changes, release notes, compatibility, licences, and security
advisories. Automate update proposals, not unconditional merging.

### 7.3 Environments and secrets

`.venv/` is disposable and must not be committed. Python's official `venv`
documentation likewise treats environments as isolated, disposable, and
recreated rather than moved. Store secrets in a developer secret manager,
deployment environment, or short-lived identity mechanism—not `.env` committed
to Git. A local `.env` may be ignored for convenience, but provide a redacted
`.env.example` containing names and documentation only.

## 8. Python architecture rules

### 8.1 Keep entry points thin

`cli.py`, HTTP handlers, queue consumers, and scheduled jobs should:

1. parse and validate external input;
2. establish request/job context;
3. call an application use case;
4. translate domain results to the external protocol; and
5. emit structured operational evidence.

Business rules belong in importable functions or classes that can be tested
without a network, database, process environment, or wall clock.

### 8.2 Point dependencies inward

A practical dependency direction is:

```text
entry points -> application use cases -> domain
                     |
                     v
              declared interfaces
                     ^
                     |
                  adapters
```

The domain must not import an HTTP framework, database driver, cloud SDK, or CLI
parser. Application code defines the interfaces it needs; adapters implement
them. Do not create an interface for every class—create one where ownership,
testing, replacement, or external behavior justifies it.

### 8.3 Model data explicitly

- Use immutable values where mutation is unnecessary.
- Separate untrusted input models from internal domain types.
- Validate at system boundaries, then rely on internal invariants.
- Use UTC instants for storage and transport; apply user time zones at display
  boundaries.
- Distinguish text (`str`) from bytes (`bytes`).
- Make identifiers distinct when mixing them would be dangerous.
- Treat database schemas, serialized messages, and public function signatures as
  compatibility contracts.

### 8.4 Configuration

Create one typed configuration object at startup. Define source precedence, for
example:

1. safe built-in defaults;
2. version-controlled static configuration;
3. deployment environment values;
4. secret-manager values; and
5. explicitly approved command-line overrides.

Validate configuration before serving traffic. Never read arbitrary environment
variables throughout domain code. Include static configuration in release
testing; Google’s CI guidance notes that configuration is a frequent source of
production failures and should be versioned and reviewed with code.

### 8.5 Errors

- Raise exceptions for exceptional conditions, not normal branching.
- Catch only errors you can handle, translate, retry, or enrich.
- Preserve causal chains with `raise ... from error`.
- Define stable domain exceptions where callers need semantic handling.
- Do not expose stack traces, credentials, queries, tokens, or personal data to
  users.
- Do not use `assert` for input validation or essential runtime behavior; Python
  may remove assertions. Assertions are appropriate in tests.
- Document retryability and idempotency at network and job boundaries.

### 8.6 Resources and concurrency

- Use context managers for files, locks, database transactions, and clients.
- Pass explicit timeouts to every network operation.
- Bound concurrency, queues, memory, and retries.
- Use exponential backoff with jitter only for retryable operations.
- Make retried writes idempotent or protect them with idempotency keys.
- Avoid mutable global state.
- Do not assume built-in operations are atomic across Python implementations.

## 9. Apply the Google Python Style Guide

Read the complete [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html)
and make deviations explicit in `CONTRIBUTING.md`. Essential rules include:

- run a linter and do not suppress warnings casually;
- import packages and modules rather than individual classes/functions, with
  documented exceptions for typing;
- use full package paths and avoid relative imports;
- keep imports at the top, grouped by future, standard library, third-party, and
  repository packages;
- avoid mutable default arguments;
- avoid mutable global state;
- use context managers for stateful resources;
- use four spaces, never tabs;
- target 80-character lines, allowing documented formatter/import/URL
  exceptions;
- use triple-double-quoted docstrings and document public behavior;
- prefer clear, unabbreviated names;
- use `lower_with_under`, `CapWords`, and `UPPER_WITH_UNDER` consistently;
- keep functions focused and reconsider functions above roughly 40 lines;
- put executable work in `main()` rather than import-time side effects; and
- type-check new and changed public APIs.

Automation removes style debate, but it does not replace judgment about API
clarity, ownership, complexity, and correctness.

## 10. Testing strategy

### 10.1 Test levels

| Level | Purpose | Properties | Typical frequency |
|---|---|---|---|
| Unit | Focused domain or small application behavior | Fast, hermetic, deterministic | Risk-based; use when it adds confidence |
| Contract | Producer/consumer agreement | Versioned examples or schemas | Every affected change |
| Integration | Real database, queue, HTTP client, filesystem, or framework boundary | Isolated environment; controlled dependencies | Primary confidence at important boundaries; CI and pre-release |
| End-to-end | Critical user journey | Few, high-value, production-like | CI subset and staged release |
| Performance | Latency, throughput, memory, scaling | Stable environment and budgets | Scheduled and release gate |
| Resilience | Timeouts, retries, partial failure, recovery | Fault injection or controlled simulation | High-risk changes and scheduled |

### 10.2 Test design rules

- Test externally meaningful behavior, not private implementation steps.
- A bug fix begins with a test that fails for the bug when practical.
- Keep behavior and its tests in the same pull request.
- Use explicit Arrange–Act–Assert structure when it improves readability.
- Give tests names such as `test_method_state_expected_result`.
- Make time, randomness, IDs, and external I/O controllable.
- Prefer small fakes at your own interfaces over deep mocks of implementation.
- Assert important outputs and side effects, not every intermediate call.
- Do not allow tests to access production services or real customer data.
- Quarantine is temporary: every skipped, flaky, or expected-failure test needs a
  reason, owner, and removal condition.
- Prefer a few high-value integration tests over exhaustive unit-test coverage.
- Cover the main workflows, important failure paths, and selected rare edge cases
  only when their impact justifies the maintenance cost.
- Review test code as carefully as production code. Google’s review guidance
  explicitly notes that tests do not test themselves.

### 10.3 Commands

```bash
# Fast local feedback
uv run ruff check .
uv run ruff format --check .
uv run mypy src tests
uv run pytest tests/integration

# Full presubmit
uv run pytest
uv build

# Verify the built wheel in a clean environment
uv run --isolated --with dist/example_service-*.whl python -c "import example_service"
```

Adapt the final wheel command to the exact artifact name or use a reviewed
verification script; shell glob behavior differs by platform.

pytest recommends `src/` layout and `--import-mode=importlib` for new projects.
See [pytest good integration practices](https://docs.pytest.org/en/stable/explanation/goodpractices.html).

## 11. One-command developer experience

Document canonical commands in `CONTRIBUTING.md`. Every developer and AI agent
must be able to discover how to:

```bash
uv sync                         # bootstrap/update environment
uv run ruff format .            # format
uv run ruff check --fix .       # safe lint fixes; inspect diff
uv run mypy src tests           # types
uv run pytest tests/unit        # fast tests
uv run pytest                   # complete tests
uv build                        # package
python scripts/validate-development-workflow.py
```

If commands become complex, add a small cross-platform task runner or Python
tool under `tools/`. The wrapper must call the same underlying tools used in CI;
do not create a second hidden implementation of the build.

## 12. Git, branches, and pull requests

### 12.1 Repository configuration

- Default branch: `main`.
- Protect `main`; disallow direct pushes except controlled automation.
- Require passing CI and current review.
- Require code-owner or domain approval for protected areas.
- Dismiss or refresh approval when material code changes.
- Require conversation resolution.
- Prevent force-push and deletion.
- Define an emergency path that is fast but still reviewed and audited.

### 12.2 Branch model

Use short-lived branches:

```bash
git switch main
git pull --ff-only
git switch -c feature/short-purpose
```

Integrate at least daily when the project allows it. Hide incomplete behavior
behind safe feature flags rather than maintaining long-lived feature branches.
Google’s version-control guidance describes trunk-based development as a
scalable policy; DORA similarly associates small batches and frequent mainline
integration with better delivery performance.

### 12.3 Change size

One pull request should have one review purpose and contain its tests and
documentation. Google’s published guidance says roughly 100 lines is often a
reasonable change and 1,000 is usually too large, while emphasizing that
conceptual focus matters more than a hard number. This repository’s `build-change`
skill uses a softer warning around 400 non-generated changed lines or eight files.

Split:

- behavior-preserving refactor from behavior change;
- API/schema contract from consumers when each intermediate state works;
- generated changes from generator logic;
- configuration/flag activation from dormant implementation; and
- independent vertical user outcomes.

Never split so finely that the repository is broken or an unused, misleading API
is merged.

### 12.4 Pull-request description

Every nontrivial change states:

- problem and intended outcome;
- scope and non-goals;
- brief/design/work-item links;
- important decisions and alternatives;
- behavior and files changed;
- validation commands and results;
- security/privacy/data/compatibility impact;
- rollout and rollback; and
- screenshots, traces, or benchmark evidence where relevant.

## 13. CI from the first change

Create `.github/workflows/ci.yml`. The example uses current major action tags for
readability. High-assurance repositories should pin third-party actions to
reviewed commit SHAs and let an update bot propose changes.

```yaml
name: ci

on:
  pull_request:
  push:
    branches: [main]

permissions:
  contents: read

concurrency:
  group: ci-${{ github.ref }}
  cancel-in-progress: true

jobs:
  quality:
    runs-on: ubuntu-latest
    timeout-minutes: 15
    steps:
      - uses: actions/checkout@v6
      - uses: astral-sh/setup-uv@08807647e7069bb48b6ef5acd8ec9567f424441b # v8.1.0
        with:
          enable-cache: true
          python-version: "3.12"
      - run: uv sync --locked --all-groups
      - run: uv run --locked ruff format --check .
      - run: uv run --locked ruff check .
      - run: uv run --locked mypy src tests
      - run: uv run --locked pytest
      - run: uv build
```

Production CI should additionally:

- run supported Python/OS matrices for libraries;
- test migrations against representative snapshots;
- scan dependencies, source, secrets, licences, and containers;
- produce machine-readable test evidence; collect coverage only when it is a
  useful diagnostic for a specific risk;
- separate trusted release jobs from untrusted pull-request jobs;
- use minimum `GITHUB_TOKEN` permissions;
- never expose secrets to forked or untrusted code;
- make failure output actionable; and
- run relevant post-merge integration and end-to-end tests.

## 14. Security and supply chain

At minimum:

- provide `SECURITY.md` with a private reporting route;
- enable secret scanning and dependency alerts;
- protect workflow files with `CODEOWNERS`;
- pin CI actions and deployment tools by immutable digest/SHA where risk warrants;
- generate an SBOM for release artifacts;
- build releases on a controlled hosted runner from a reviewed commit;
- sign artifacts and publish provenance;
- use short-lived federated credentials such as OIDC rather than stored cloud
  keys; and
- verify provenance before promotion in high-assurance environments.

SLSA defines progressive source/build supply-chain guarantees and provenance
formats. Use [SLSA v1.2](https://slsa.dev/spec/v1.2/) as a maturity model rather
than claiming compliance without verifying every requirement.

## 15. Releases and operations

### 15.1 Build once

Create the wheel, container, or executable once from a reviewed commit. Record:

- Git commit and source repository;
- build workflow identity;
- dependency lock digest;
- artifact digest;
- configuration/migration version;
- test and scan evidence; and
- provenance/signature.

Promote that artifact through environments; do not rebuild different bits for
staging and production.

### 15.2 Versioning

For published APIs/packages, use semantic versioning when it accurately
communicates compatibility. Define deprecation duration and removal policy.
Applications may use immutable build identifiers while still maintaining a
human-readable release version.

### 15.3 Production services

Before launch, define:

- structured logs with correlation identifiers and redaction;
- request, error, latency, saturation, queue, and business metrics;
- distributed tracing where it adds diagnostic value;
- dashboards and actionable alerts;
- SLOs and an error budget;
- health/readiness behavior;
- capacity expectations;
- backup/restore and disaster recovery;
- on-call ownership and runbooks; and
- feature-flag and rollback procedure.

Use canary or staged exposure when the blast radius justifies it. Google SRE
defines canarying as a partial, time-limited deployment evaluated before
continuing. See [Canarying Releases](https://sre.google/workbook/canarying-releases/).

## 16. Integrate the human–AI workflow

Use the lifecycle skills and optional guided facade according to work size:

| Situation | Skills |
|---|---|
| Local low-risk fix | `build-change` -> human review; add `review-change`, `pr-review`, `clean-code-review`, or `critique-review` when useful |
| Normal feature | `define-product` -> optional `design-solution` -> `build-change` -> `review-change` -> proportionate rollout |
| New greenfield product, guided | `specify-project` -> `plan-delivery` -> build/review loops -> `launch-product` |
| New product, modular | `define-product` -> `design-solution` -> all later lifecycle skills through `launch-product` |
| Multi-developer milestone | add `plan-delivery`; agree contracts before parallel work |

`specify-project` is a facade over product definition and solution design, not an
additional delivery stage. Use its root `SPECIFICATION.md` for a single-product
repository or `docs/product/<slug>/SPECIFICATION.md` in a multi-product
repository. The specification remains Draft until its product frame and
technical design are separately accepted by accountable humans.

The AI must inspect this exact repository before suggesting files or APIs. It may
implement and validate one bounded change, but it must stop for product,
architecture, API, data, security, dependency, or destructive decisions. A new
request does not silently supersede an accepted design. The implementing AI
does not approve its own change, and humans retain merge and release authority.

See [Idea to Production in the Modern AI Age](IDEA-TO-PRODUCTION-HANDBOOK.md)
for the complete operating model.

## 17. Definition of done

A Python change is done only when:

- acceptance behavior is implemented;
- non-goals remain out of scope;
- public APIs and data contracts are reviewed and documented;
- tests would fail if the new behavior broke;
- formatting, linting, typing, tests, and packaging pass;
- security, privacy, licence, compatibility, and migration effects are addressed;
- documentation and examples are current;
- operational signals and rollback are ready when applicable;
- the complete diff has independent human review;
- the exact reviewed commit is identified; and
- rollout evidence—not merely merge status—supports calling the feature shipped.

## 18. Common failure modes

| Failure | Prevention |
|---|---|
| Imports work locally but not from the wheel | `src/` layout; test installed artifact |
| “Works on my machine” dependencies | committed lock; `--locked` CI; disposable environments |
| Huge AI-generated pull request | one-purpose work item; small-batch warning; human plan checkpoint |
| Tests mirror implementation and miss bugs | behavior assertions; mutation/property testing for critical logic; independent review |
| Secrets in Git or CI logs | secret manager, least privilege, redaction, scanning |
| Flaky CI normalized as noise | owner, diagnostic evidence, fix/quarantine deadline |
| Silent breaking API/schema change | accepted contract, compatibility tests, version/migration plan |
| Import-time network/config failure | thin entry point; explicit startup; dependency injection |
| Manual release cannot be reproduced | hosted build, artifact digest, provenance, automated promotion |
| Documentation drifts | canonical docs beside code, ownership, reviewed updates |

## 19. Authoritative references

- [Google Python Style Guide](https://google.github.io/styleguide/pyguide.html)
- [Software Engineering at Google](https://abseil.io/resources/swe-book)
- [Google Engineering Practices: Code Review](https://google.github.io/eng-practices/review/)
- [Google Engineering Practices: Small CLs](https://google.github.io/eng-practices/review/developer/small-cls.html)
- [Python Packaging User Guide](https://packaging.python.org/)
- [Writing `pyproject.toml`](https://packaging.python.org/en/latest/guides/writing-pyproject-toml/)
- [`src` layout vs flat layout](https://packaging.python.org/en/latest/discussions/src-layout-vs-flat-layout/)
- [Python virtual environments](https://docs.python.org/3/library/venv.html)
- [uv project documentation](https://docs.astral.sh/uv/guides/projects/)
- [Ruff documentation](https://docs.astral.sh/ruff/)
- [pytest good integration practices](https://docs.pytest.org/en/stable/explanation/goodpractices.html)
- [mypy documentation](https://mypy.readthedocs.io/en/stable/)
- [SLSA specification](https://slsa.dev/spec/v1.2/)
- [Google SRE: Canarying Releases](https://sre.google/workbook/canarying-releases/)

## 20. Companion guides

- [Google-Inspired Language-Agnostic Project Handbook](LANGUAGE-AGNOSTIC-PROJECT-HANDBOOK.md)
- [Idea-to-Production Handbook](IDEA-TO-PRODUCTION-HANDBOOK.md)
- [Product Development Workflow](../for-human/development-guide.md)
- [Four Common Workflow Recipes](../WORKFLOW-COOKBOOK.md)
- [AI Agent Workflow](../for-ai/ai-agent-guidelines.md)

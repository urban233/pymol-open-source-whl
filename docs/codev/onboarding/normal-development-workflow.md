# Normal Development Workflow

Use this guide when you have a normal bug fix, small feature, or planned work
item to deliver. It shows the commands you run and the points where you ask
your AI assistant to investigate, build, and review. See
[onboarding-guide.md](onboarding-guide.md) for the workflow's principles.

## 1. Install CoDev once per repository

Install the CLI outside the repository, then install its workflow bundle into
the repository. `pipx` and `uv tool` keep CoDev out of your product's runtime
dependencies.

```shell
pipx install open-codev-workflow
# or: uv tool install open-codev-workflow

codev init --target . --agent-platform all
codev status --target .
```

Use one platform instead of `all` when appropriate, for example
`--agent-platform opencode`. Commit the installed workflow files, including
`.codev/lock.json`, with the repository. You do not run `init`, `update`, or
`remove` while product code is being built.

## 2. Describe the work to your AI assistant

Start an AI session in the repository and state the outcome in plain language.
You do not need to choose a skill yourself. For example:

```text
Fix the checkout total when a discount is applied to a tax-exempt item. The
total must include shipping but not tax. Add a regression test.
```

The assistant begins with **Understand**. It inspects the repository and, for
a routine change, presents a short focus card covering the change, success
criteria, non-goals, permitted files, validation, and stop conditions.

Answer only decisions that affect outcome, scope, design, risk, or ownership.
For a local, low-risk change, let it proceed to **Build**. If the work changes
a public/shared contract, persistent data, permissions, privacy, or security,
settle the design first. If multiple developers will work concurrently, agree
on the shared contract and delivery plan before implementation starts.

## 3. Start and branch a tracked task

For normal tracked work, capture the current commit as the base, then start the
item and create its branch. Pick a stable, descriptive ID such as
`checkout-tax-exempt-total`.

```shell
git rev-parse HEAD
codev task start --id checkout-tax-exempt-total --base <base-sha> \
  --summary "Correct tax-exempt checkout totals" --link <issue-or-plan-url>
codev git branch --id checkout-tax-exempt-total --base <base-sha>
```

`--link` points to the issue, brief, design, or plan that authorizes the work.
For work already represented by a GitHub issue, use `--github-issue <number>`
instead of separately supplying its link and summary. An AI-driven
`orchestrator` normally performs the lifecycle recording and invokes a bounded
builder and fresh reviewer; these commands are useful when you are driving the
workflow manually or checking its state.

If you started coding before involving CoDev, use one entry mode deliberately:

```shell
# Unfinished work: let the build loop continue the existing diff.
codev task start --id checkout-tax-exempt-total --base <base-sha> \
  --entry takeover --summary "Correct tax-exempt checkout totals"

# Finished work: skip the build loop and send it to the PR review path.
codev task start --id checkout-tax-exempt-total --base <base-sha> \
  --entry direct-review --summary "Correct tax-exempt checkout totals"
```

## 4. Build in small, evidence-backed rounds

Ask the assistant to implement the accepted focus card. It should inspect the
actual code and tests before editing, keep the diff bounded, run the relevant
formatter, static checks, and tests, then inspect the complete diff. It stops
and asks you for one decision if the work needs a material design or scope
change.

At the end of the build round, require an evidence receipt that names the files
changed, exact validation commands and results, acceptance evidence, known
limitations, and review state. The workflow records builder and reviewer
evidence under `.codev/task/`; it never writes product source itself.

Check progress at any time:

```shell
codev task status --target .
codev task log --id checkout-tax-exempt-total --target .
codev task check --id checkout-tax-exempt-total --head "$(git rev-parse HEAD)"
```

Do not accept self-approval. The implementer can self-check, but a fresh,
independent reviewer must inspect the exact diff. If a reviewer finds a
bounded defect, send it back through Build, correct it, and obtain a new review
of the changed snapshot.

## 5. Commit and open the pull request

Once `codev task check` reports `ok_ready_for_pr`, use the guarded Git commands
for the task. They ensure the action occurs on that item's branch rather
than the default branch.

```shell
codev git commit --id checkout-tax-exempt-total \
  --message "Fix tax-exempt checkout total"
codev git push --id checkout-tax-exempt-total
codev git open-pr --id checkout-tax-exempt-total \
  --title "Fix tax-exempt checkout total"
```

The pull request opens as a draft. Run or request the outer review loop for the
open PR. It gathers PR and CI evidence and dispatches specialist reviews for
correctness/tests, security/data, concurrency, architecture/maintainability,
and rollout. For an already-open PR, ask the assistant to review that exact PR;
do not review a moving local diff instead.

When the evidence is complete and the review loop says the item is ready, mark
the PR ready for human review:

```shell
codev git mark-ready --id checkout-tax-exempt-total
```

## 6. Make the human decisions and close the item

You inspect the exact pull request, review evidence, and CI results. You decide
whether to request changes, approve, merge, deploy, or expand rollout. CoDev
and the AI provide evidence but do not make those decisions.

After the human approval and merge decision, record the final outcome:

```shell
codev task close --id checkout-tax-exempt-total --outcome approved
```

Use `--outcome abandoned` when the work is intentionally stopped, or
`--outcome escalated` when it needs an unresolved human decision. Check the
repository-wide work state before starting another item:

```shell
codev status --target .
```

## Daily command checklist

```shell
# Once per repository
codev init --target . --agent-platform all

# One normal task
git rev-parse HEAD
codev task start --id <id> --base <base-sha> --summary "<outcome>" --link <url>
codev git branch --id <id> --base <base-sha>

# During build and review
codev task status
codev task check --id <id> --head "$(git rev-parse HEAD)"

# When ready for a pull request
codev git commit --id <id> --message "<message>"
codev git push --id <id>
codev git open-pr --id <id> --title "<title>"
codev git mark-ready --id <id>

# After the human decision
codev task close --id <id> --outcome approved
```

To update the installed workflow later, preview first and resolve any reported
local changes rather than overwriting them:

```shell
codev diff --target .
codev update --target .
```

---
name: design-skill-eval
description: Guide a developer through designing and scaffolding one new task for an installed skill's performance-evaluation corpus -- pick a falsifiable ground truth, write a prompt that never names the skill, a deterministic verifier, and a judge rubric, then tag it with the right skill and category so `codev eval benchmark run` discovers it. Use when adding eval coverage for an existing skill, or bootstrapping the first task for a skill that has none. Do not use to run an existing benchmark, to build or edit the skill under test itself, or for general code review.
license: BSD-3-Clause
---

# Design Skill Eval

Help a developer turn "I want to know if skill X actually works" into one
committed task under `.codev/eval/tasks/<name>/` that `codev eval task run` and
`codev eval benchmark run` can execute. The mechanical scaffolding
(`codev eval task create`) is the easy part; the design decisions below
are what make the resulting task worth running at all. Read
`references/eval-design-checklist.md` before writing prompt.md, verifier.json,
or rubric.md -- it has the failure modes that make a task worthless even
though it validates cleanly.

## 1. Confirm this is the right path

Use this skill to add or design one eval task for a skill that already
exists in this repository.

Redirect instead when:

- the developer wants to *run* tasks that already exist -- point them at
  `codev eval task run <name>` or `codev eval benchmark run <skill>`;
- the developer wants to build or change the skill itself, not evaluate it --
  that is ordinary `build-change` work on `.agents/skills/<name>/`;
- the request is a normal code review -- use `review-change`.

## 2. Understand the skill under test

Read the skill's `SKILL.md` completely: what does it promise, what does a
correct outcome look like, and what would a plausible failure look like?
Then inspect `.codev/eval/tasks/*/task.json` for existing tasks already
tagged with this skill (`skill` field). Note their `category` values --
reuse an existing category if this task genuinely tests the same
dimension, or add a new one only if it doesn't. Don't invent a category name
that duplicates an existing one under a different spelling.

If this is the skill's *first* task, look at `.codev/eval/tasks/seeded-defect-*`
for `review-change` as a worked example of the seeded-defect pattern before
deciding whether it fits the skill you're evaluating.

## 3. Choose the ground-truth mechanism

This is the real design decision, and it determines everything downstream.

**Seeded-defect (falsifiable, deterministic) -- prefer this when possible.**
Seed the repository with one deliberately planted, specific, checkable
problem (a bug, a missing check, a wrong value, a broken contract). The
verifier is a small script that greps or parses the actor's output for
evidence the *specific* planted problem was addressed -- not just "did the
actor do something plausible." This is what every `seeded-defect-*` task
does for `review-change`.

**Rubric-only (open-ended) -- accept this only when a seeded task
genuinely isn't constructible.** Some skills produce output whose quality
isn't reducible to "did it catch one specific thing" (open-ended writing,
synthesis, exploratory design). Here the verifier can only check structural
validity (the output exists, parses, matches a schema), and the judge's
rubric carries the entire quality signal. Say explicitly in the task's
`description` that this task has no deterministic ground truth, so
nobody mistakes a passing judge verdict for the stronger guarantee a seeded
task gives.

Do not pick rubric-only by default because it's easier to write. It produces
a materially weaker task -- read
`references/eval-design-checklist.md#ground-truth` before deciding.

## 4. Pick a category

A category groups tasks for aggregation in `codev eval benchmark run`'s
report -- it is not one-task-per-category by rule, just by convention so
far in this repository. Multiple tasks can share a category; the
benchmark's percentage for that category is the aggregate pass rate across
all of them. Pick a name that describes the *dimension being tested*
(`security`, `error_handling`, `citation_accuracy`), not the task's
specific scenario.

## 5. Scaffold the task

`codev` is a real command, already installed and on `PATH` in this
environment -- run it directly as a shell command, the same way you would
run `git`. Do not hand-write `task.json` from scratch: `task.json`
accepts *only* `schema_version`, `name`, `description`, `skill`,
`category`, `actor_timeout_seconds`, and `judge_timeout_seconds` --
`validate_task()` rejects it outright for a missing required field or an
invented extra one (e.g. a `"prompt": "prompt.md"` field pointing at the
other files by name -- the filenames are fixed by convention, not declared
in the manifest). Observed directly, twice, from two different actors: one
otherwise well-designed task failed validation for a missing timeout
field after being hand-written instead of scaffolded; another failed for
inventing manifest fields that don't exist in the real schema. Both mistakes
disappear if you run the command below instead of guessing the file's shape.

```bash
codev eval task create <name> --target . --include <path> --include <path>
```

`--target` must be an existing Git repository; `--include` selects the
specific files that become the task's `repository/` seed (repeat the
flag per file or directory). This writes a starter `task.json`,
`prompt.md`, `rubric.md`, and `verifier.json` under
`.codev/eval/tasks/<name>/` for you to edit -- it does not infer a working
task on its own. `--include` requires paths that already exist in
`--target`; brand-new seed content (a toy target file, a bespoke verifier
script) still needs to be written by hand after scaffolding runs.

## 6. Design the seed repository

Keep `repository/` small and self-contained: only the files genuinely
needed for the scenario, no `.git`, no secrets, no symlinks, no external
dependencies, no assumption of network access. The actor will get a fresh
Git worktree built from exactly this seed and nothing else it doesn't
discover on its own.

## 7. Write `prompt.md`

The prompt must describe the task on its own terms and never name the skill
being evaluated. Staging (or not staging) `.agents/skills/<skill>/` into
the worktree is the *only* thing that should differ between the with-skill
and baseline conditions (see `_stage_skill()` in `src/codev_workflow/eval.py`
if you want the mechanism, not just the rule); a prompt that says "follow
the X skill" defeats the comparison before it starts.

If the actor must produce output in a specific machine-checked shape,
**spell out the exact schema in the prompt itself** -- field names, allowed
values, format. Don't assume the actor can infer or already knows a
project-specific contract. This exact mistake silently broke this
project's own judge step earlier: the prompt said "return the required
JSON" without ever stating what "required" meant, and the model spent its
whole turn exploring instead of answering. Assume nothing is known that
isn't written down in this file.

## 8. Write the deterministic verifier

A task declares exactly one of `verifier.json` or `checks.json` -- never
both, never neither.

`verifier.json` is `{"schema_version": 1, "command": [...], "timeout_seconds": N}`
-- an argv array, not a shell string. Use only standard-library tooling; do
not assume network access or installed dependencies. Import
`codev_workflow.eval_checks`'s shared helpers (`load_structured_output`,
`finding_matches`, `changed_paths_since_seed`, `require`) instead of
reimplementing JSON-loading or finding-matching by hand -- this works
because the verifier subprocess runs under the same interpreter as CoDev
itself.

`checks.json` is the declarative alternative, for the common cases: a list
of checks (`json_field_equals`, `finding_matches`, `files_unchanged_except`,
`command_succeeds`) expressed as data instead of a script. Prefer it unless
the task needs something none of those four cover --
`.codev/eval/tasks/audit-google-python-style-demo/checks.json` is a real
worked example.

Either way, the verifier must fail when the specific planted problem is
missed, not merely when the actor crashes or produces no output at all -- a
verifier that only checks "did something get written" cannot distinguish a
real pass from a lucky one.

## 9. Write `rubric.md`

The judge never sees the actual worktree or code -- only `rubric.md` and
the captured evidence (`actor-output.txt`, `diff.patch`, verifier
stdout/stderr). Every rubric criterion must be answerable from that
evidence alone. A criterion that requires re-inspecting the source (e.g.
"is the fix idiomatic for this codebase") cannot be judged fairly and will
produce noise, not signal.

## 10. Tag the task

Set `"skill"` and `"category"` in `task.json` (required fields --
`validate_task()` rejects a task missing either). These are what
`codev eval benchmark run <skill>` uses to discover and group the task;
without them the task cannot be run as part of a benchmark at all.

## 11. Prove the task actually discriminates

Before treating the task as done, run it both ways:

```bash
codev eval task run <name> --target . --output <dir-with-skill>
codev eval task run <name> --target . --output <dir-baseline> --baseline
```

A task that passes in both conditions, or fails in both, carries no
signal about the skill -- it's a tautology, not an eval. You want a
realistic chance that baseline fails and with-skill passes; if that
isn't true, the planted problem is either too easy (any capable model
avoids it unprompted) or too hard (the skill alone can't reliably prevent
it). Adjust the scenario, not the pass condition, to fix this.

## 12. Confirm corpus integration

Run `codev eval benchmark run <skill> --category <this-category> --repetitions 1`
once to confirm the task is discovered and reports cleanly before
committing it. Mention the live-model cost to the developer before running
anything beyond this one-repetition check -- see
`docs/features/skill-eval/README.md` for the cost shape of a full benchmark.
Add `--agent <fake-agent-stub>` to run this same discovery/reporting check
for free first, before spending any real model budget on it.

Note that this scoped check uses `--category`, so it never touches the
skill's own packaged eval trace (`--category`-restricted runs never package,
by design -- see `docs/adr/0028-skill-packages-carry-their-own-eval-trace.md`).
Once the full, unrestricted benchmark eventually runs, it writes
`.agents/skills/<skill>/evals/benchmark.json` and `evals/BENCHMARK.md`
automatically; `codev eval show <skill>` renders that trace back as text.

## Stop conditions

Stop and ask for one precise decision when:

- the skill under test has no clear definition of a correct outcome to seed
  a defect against;
- the developer wants a rubric-only task and hasn't been told about the
  weaker guarantee that implies;
- the planted problem can't be checked without either network access or a
  dependency beyond the standard library; or
- the task's with/without run (step 11) doesn't discriminate and the
  developer wants to ship it anyway.

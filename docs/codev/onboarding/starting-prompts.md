# Starting Prompts

Two prompts you'll type often: kicking off the next task, and starting
outer-loop review on an open pull request. Both assume CoDev is installed
and you're about to switch your assistant's primary agent to `orchestrator`
or `outer-loop-runner` (on OpenCode, `/agent orchestrator` or `/agent
outer-loop-runner`; other platforms select the agent their own way).

Fill in the bracketed parts. Everything else is meant to be typed as-is —
these are deliberately short. `orchestrator` and `outer-loop-runner` ground
themselves in the repository and ask you only for decisions that are
actually yours to make; a longer prompt does not make either one safer, and
mostly just repeats what they already do by default.

## Starting the next task

```text
Take task <WORK-ITEM-ID> from the delivery plan at <PATH-OR-LINK>.
GitHub user for ownership/assignment is <YOUR-GITHUB-LOGIN>. If the plan
still has placeholder owner/reviewer names that haven't been assigned yet,
disregard them and use the login above instead.

Create the task's own branch, then stop and show me the implementation
plan before any code changes — I want to discuss and accept it first, not
after the fact.
```

Why this is enough, and no more:

- **You don't need to ask for a GitHub issue explicitly.** `orchestrator`
  checks for one itself before opening round state, creates it if this
  repository tracks issues on GitHub and none exists yet, and `codev task
  start` refuses to proceed silently without one — you'll be told directly
  if it can't resolve this on its own instead of finding out later that a
  pull request never linked back to anything.
- **Name the real GitHub user up front.** Ownership/reviewer fields in a
  delivery plan are often still placeholders (`Developer 1`, a role name,
  whatever the plan template shipped with) — naming the actual login once
  here means it's the only one used, everywhere it matters, without you
  having to repeat the correction later.
- **"Show me the plan before any code changes" is the one instruction worth
  always including.** It's already the default for delegated, multi-session,
  or higher-risk work, but stating it removes any doubt for a borderline
  case and guarantees a checkpoint even for a smaller item.
- **If this task's pull request needs a specific integration branch**
  (not the repository's default — a `plugins` line, a `develop` branch,
  whatever this repository actually uses), say so before the branch is
  pushed: "target `<branch>`, not `<default>`." Set it once, repository-wide,
  with `codev config set git.pr_base <branch>` instead of repeating it on
  every task.

## Starting outer-loop review

```text
Start outer-loop review for PR #<NUMBER>. Present the five specialists as a
numbered menu and wait for my selection before dispatching anything — don't
run a fresh five-specialist pass on your own judgment.
```

Why the second sentence is there: this is a known, actively mitigated gap.
`outer-loop-runner` is already instructed to present the menu, but nothing
can force a model to render it before invoking a specialist, and doing
exactly that — skipping straight to a full five-specialist pass — has been
observed in real sessions. On OpenCode, each specialist dispatch also
requires its own explicit permission confirmation as a mechanical backstop,
independent of what gets narrated — you'll see one prompt per specialist
regardless. Restating the expectation here costs one sentence and closes
the gap the rest of the time, on every platform.

If you'd rather run every dimension without narrowing it down, say `all`
when asked, or state it in the same message: "...run all five." Either way,
CoDev records exactly which specialists actually ran, distinctly from what
was merely asked, in `codev task log`.

# CoDev Onboarding Guide

*Start here.* This is the short version — what CoDev does, the mental model
behind it, and what a day of work actually looks like. For the full
technical map (every command, skill, and agent), see the CoDev project's
`docs/product-map.md`. For a command-led daily workflow, see
[normal-development-workflow.md](normal-development-workflow.md). For worked
walkthroughs, see [examples.md](examples.md).

## The problem

An AI can now write plausible code faster than a human can review it. A
sequence aligner that silently returns coordinates in the wrong system, a
molecule parser that "fixes" a failing unit test by loosening its
assertion, a docking pipeline that quietly grows from a bug fix into a
rewrite — all fluent enough that a tired reviewer skims past it. CoDev
exists to make AI-assisted development fast *without* being unaccountable.

## The mental model

Think of CoDev as a narrow waist between what you actually want (an
outcome, a design decision, a bug report) and the diff that delivers it.
Three ideas carry almost all of its behavior:

- **Repository grounding.** An AI's claims about how the code works are
  worth nothing until checked against the actual files. Every skill starts
  with inspection, not proposal.
- **One fact, one owner.** Outcome and scope live in a brief. Architecture
  lives in a design document. Assignment and status live in a delivery
  plan. Behavior lives in code and tests. Nothing gets copied between
  documents — later documents *link* to earlier ones.
- **Small, reviewable, evidence-backed changes.** A change is done when
  it's small enough to review, validated with commands you can rerun, and
  an independent reviewer — human, and automatically also AI — has looked
  at the *exact* diff.

If you remember nothing else: **the AI supplies evidence, you supply
authority.**

## The four steps you'll actually see

You never have to name a skill. Describe what you want, and CoDev resolves
it to one of four steps:

| Step | Question it answers | A concrete instance |
|---|---|---|
| **Understand** (`define-product`) | What are we building, and what has to be decided first? | "Our alignment pipeline only reads BAM. A collaborator's lab produces CRAM files instead — should we add a reader, or require conversion upstream?" |
| **Build** (`build-change`) | What's the smallest change that delivers this, with evidence it works? | Fixing a BED-to-VCF coordinate conversion that's off by one at contig boundaries — BED is 0-based half-open, VCF is 1-based closed, and it's easy to get that one substitution wrong. |
| **Review** (`review-change`, plus an automatic pass) | Is this exact change correct, safe, and consistent with what we agreed? | Catching that a SMILES canonicalizer drops E/Z stereochemistry for one specific ring class — a real, narrow correctness bug, not a style nit. |
| **Ship** | Are we ready to expose this, and how will we know it's working? | Rolling a new ML-based hepatotoxicity filter out to 10% of incoming compound batches before trusting it on the full screening queue. |

Two of the twelve installed skills sit outside this table because they don't
fit the four-step shape: `pr-review` reviews a GitHub Pull Request that
already exists (possibly not even yours), and `critique-review` turns an
existing finding into a concrete suggested diff — it's a bridge from a
review to a fix, not a review itself. `design-skill-eval`,
`technical-writing-style`, and `testing-craft` are different kinds of
tools again: `design-skill-eval` scaffolds evaluation tasks so you can
measure whether a skill actually helps, empirically, rather than just
trusting it; `technical-writing-style` is what the other planning skills
read automatically before drafting prose, and what you can invoke directly
to revise the writing quality of an existing document; and `testing-craft`
is what `specify-project`, `design-solution`, and `build-change` read
automatically before they design or write test content, and what you can
invoke directly to design a test strategy, audit an existing test suite's
health, or triage a flaky test.

## Two steps that only show up sometimes

**Design** (`design-solution`) and **Plan** (`plan-delivery`) are not
extra stages everything passes through — they're conditional depth inside
Understand, triggered by real properties of the change: a shared API or
data contract, an authentication or privacy boundary, or more than one
developer working the same area concurrently. A one-line fix to a
coordinate-conversion helper skips both. Deciding how two developers will
split a variant-calling pipeline without colliding needs `plan-delivery`
first.

At the very edges: **Specify** (`specify-project`) is for a genuinely new
product or a whole-product redesign — one guided interview producing a
single canonical specification, used rarely. **Launch** (`launch-product`)
is for real rollout risk — staged exposure, rollback readiness, the kind of
decision the toxicity-model example above needs. Most changes never touch
either.

## Who does what

| | Human | AI |
|---|---|---|
| **Understand** | States the problem, makes the product/architecture call | Investigates the repository, proposes framing, asks one targeted question at a time |
| **Build** | Approves the plan, answers stop-condition questions | Grounds the plan in real code, implements, tests, self-checks |
| **Review** | Reads the exact diff, decides whether it may merge | Produces an independent, evidence-based review |
| **Ship** | Authorizes exposure and expansion | Assembles readiness evidence, proposes rollout stages |

## Review, in practice

Every change gets an independent human review — whoever wrote it, human or
AI, does not approve it. Where the platform supports subagents, this
mostly happens without you orchestrating it by hand: a fast correctness
check runs after each build, a style and maintainability gate runs
automatically right before a pull request opens, and once that PR exists,
five specialist reviewers — correctness, security, concurrency,
architecture, rollout — examine it in parallel and hand you exactly the
findings that need a decision. You triage; CoDev does not decide for you
which finding matters. `review-change` still exists for the case none of
that covers: a diff with no task and no open PR yet, reviewed on
demand.

## Multi-developer, briefly

One rule carries most of it: every component or contract has one
responsible owner, and the owner and reviewer are never the same person.
Two developers can work in parallel once they agree on a shared contract —
say, a normalized variant record (chromosome, position, reference,
alternate allele) — and a fixture to test against it, rather than
discovering the mismatch after both branches are done.

## Where to go next

- Commands for a normal task:
  [Normal Development Workflow](normal-development-workflow.md)
- Copy-paste prompts for starting a task or outer-loop review:
  [starting-prompts.md](starting-prompts.md)
- Worked walkthroughs, start to finish: [examples.md](examples.md)
- The full command/skill/agent reference: `docs/product-map.md`
- How the bundle installs and updates: `docs/architecture.md`

# Eval design checklist

Concrete failure modes observed while building this project's own
`review-change` eval corpus, kept here so the next task doesn't repeat
them. Read the relevant section before writing the file it names.

## Ground truth {#ground-truth}

The strength of a task is entirely a function of how falsifiable its
ground truth is. Three tiers, strongest first:

1. **Seeded defect + deterministic verifier.** A specific, plantable
   problem; a verifier that checks for evidence *that exact problem* was
   addressed, not just "the actor did something." This is the only tier
   that gives you a hard pass/fail number you can trust without a human
   rereading transcripts.
2. **Structural verifier + judge rubric.** The verifier only confirms the
   output parses/exists/matches a schema; the judge's rubric carries the
   real quality signal. Weaker, because the judge is itself a sampled model
   call and can be wrong or inconsistent between runs.
3. **Judge rubric alone, no verifier gate.** Avoid this if at all possible.
   Nothing stops a plausible-sounding but wrong answer from passing.

If you're tempted to reach for tier 2 or 3 because tier 1 is hard to
construct for this skill, that difficulty is itself useful information --
it may mean the skill's actual value is hard to measure, which is worth
saying explicitly in the task's `description` rather than papering over
with a softer eval.

## The tautology trap

A task that always passes, or always fails, regardless of whether the
skill is staged, is worse than no task -- it produces a confident-looking
number that means nothing. Concretely:

- **Too easy:** the planted problem is something any capable model avoids
  by default (an obviously named security function, an egregious typo).
  Both conditions pass; delta is always ~0 by construction, not because the
  skill doesn't help.
- **Too hard:** the problem requires context or tools the actor doesn't
  have regardless of the skill (needs to run a real test suite that isn't
  in the seed, needs external documentation). Both conditions fail.

Run step 11 of `SKILL.md` (the with/without check) *before* committing a
task, not after it's already in the corpus generating misleading
percentages.

## Prompt design

- Never name the skill under test. `_stage_skill()` in `src/codev_workflow/eval.py`
  is the only mechanism that should differ between conditions -- a prompt
  that says "use the review-change skill" makes the comparison meaningless
  because the actor is told what to do either way.
- If the actor must produce structured output, put the *exact* schema in
  the prompt, inline, with field names and allowed values spelled out.
  "Return the required JSON" is not a schema -- the actor has no way to
  know what "required" means unless you write it down. This exact bug
  shipped in this project's own judge prompt and silently produced zero
  useful judge output for every run until it was found.
- State exactly which file(s) the actor should write to and that it should
  not modify anything else. An actor with no output-location constraint
  will write review prose to chat instead of a file a verifier can read.

## Verifier design

- `verifier.json`'s `command` is an argv array executed directly, not
  through a shell -- no pipes, no globbing, no environment-variable
  expansion. If the check needs more than one command, write a small
  script and call it as the single command.
- Standard library only. The evaluation harness does not install
  dependencies, and assuming any will make the task fail for reasons
  unrelated to the skill being tested.
- Check for the specific planted problem, not just "the actor produced
  well-formed output." A verifier that only validates JSON shape will
  reward an actor that writes a syntactically valid but empty or wrong
  answer.
- Keep the verifier's exit code binary: `0` for pass, non-zero for fail.
  Print the reason to stderr for debuggability, but the harness only reads
  the exit code.

## Rubric design

- Every criterion must be answerable from `rubric.md` plus the captured
  evidence files alone (`actor-output.txt`, `actor-events.jsonl`,
  `diff.patch`, verifier stdout/stderr). The judge is deliberately never
  given the actual worktree -- it cannot go re-read the source to check a
  criterion that requires it.
- Keep criteria short and specific ("names SQL injection and explains the
  interpolation") rather than broad ("is a good review") -- a broad
  criterion just becomes the judge's own unconstrained opinion.

## Seed repository safety

- No `.git`, no secrets, no symlinks, no dependency manifests that imply
  an install step, no assumption of network access.
- Keep it small. The actor pays real time and tokens exploring the seed;
  an oversized seed makes every run of this task more expensive without
  making it a better test.

## Cost awareness

Every repetition of every condition is a real, live model call (and, if
the verifier passes, a second one for the judge). Before adding a task
to a skill's corpus, or before running a benchmark across many tasks,
estimate the cost: tasks × 2 conditions × repetitions, each run costing
roughly 20K-55K tokens based on this project's own measured runs (actor
alone when the judge is skipped; actor + judge when it's reached). Prefer
`codev eval benchmark run <skill> --category <name> --repetitions 1` for a
cheap sanity check before committing to the full corpus at the default
repetition count.

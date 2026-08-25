# Test Suite Health Reference

Distilled from *Software Engineering at Google*'s Continuous Integration
chapter and Google's published research on flaky-test and mutation-testing
infrastructure. Apply this to an existing suite as a whole -- its
reliability, speed, and trustworthiness over time -- rather than to one
test's internals (`writing-tests.md`) or to what a new change should test
(`test-strategy.md`).

## Contents

1. Hermetic tests
2. A fast tier and a slow tier
3. Flaky-test triage
4. Coverage stays a diagnostic
5. A manual proxy for mutation testing
6. Self-check before closing an audit

## 1. Hermetic tests

- A hermetic test is fully self-contained: it does not depend on another
  test's execution order, shared external state, wall-clock time, network
  access, or any other test's side effects. Running it twice, or running
  it alone versus inside the full suite, produces the same result every
  time.
- Non-hermetic tests are the single largest source of a suite losing
  trust: an intermittent failure caused by shared state is
  indistinguishable, from the outside, from a real regression, until
  someone spends the time to prove it is not one.
- When a test cannot be made hermetic because it genuinely needs a shared
  resource, isolate it explicitly -- a dedicated tier, a clearly named
  tag, or its own slower job -- rather than letting it sit unmarked among
  hermetic tests and erode confidence in all of them.

## 2. A fast tier and a slow tier

- Split a suite into a fast, reliable tier expected to run on every
  change, and a slower, broader tier that runs less often (nightly, before
  merge to a protected branch, or on demand) -- the general principle
  behind what Google calls presubmit and postsubmit, without assuming
  Google's specific infrastructure.
- The fast tier earns the right to gate every change only by staying fast
  and hermetic; a fast tier that becomes slow or flaky gets skipped or
  ignored, which defeats its purpose entirely.
- Move a test to the slow tier deliberately, with a stated reason (it
  needs a real external dependency, it is inherently slow, its flakiness
  has not yet been fixed) -- not silently, and not as a permanent home for
  a test nobody intends to speed up.

## 3. Flaky-test triage

A flaky test passes and fails on the same code, non-deterministically.
Trust in a suite erodes sharply once its flake rate crosses roughly 1%;
treat that as an action threshold, not a tolerable background rate. For a
specific flaky test, decide explicitly among:

- **Fix now**, when the root cause is quickly identifiable (a race
  condition, an unmocked clock, an ordering dependency) -- the default
  choice when the fix is bounded.
- **Quarantine and ticket**, when the fix is not quick: remove the test
  from the tier that gates changes, file a tracked issue with the failure
  evidence, and set an explicit owner and expectation -- not silent
  disablement that nobody revisits.
- **Delete**, when the test no longer earns its cost relative to what it
  catches, or duplicates coverage a more reliable test already provides.
  Deleting a bad test is a legitimate outcome, not a failure to fix it.

Never leave a known-flaky test enabled and gating changes while ignoring
its failures -- that trains engineers to re-run first and read the failure
never, which erases the value of every other test in the same tier.

## 4. Coverage stays a diagnostic

Line or branch coverage tells you where no test executes a piece of code
at all -- useful for finding a gap, worthless as a target. A team that
treats a coverage percentage as a quality gate gets tests written to move
the number, not tests written to catch a bug. This matches this
repository's own stated stance in `.codev/for-ai/ai-agent-guidelines.md`;
apply it the same way here.

## 5. A manual proxy for mutation testing

Mutation testing checks a suite's quality directly by deliberately
breaking the code and confirming a test catches the break -- a stronger
signal than coverage, but it needs infrastructure most repositories do not
have. Without that infrastructure, apply the same idea manually and
cheaply: for a test under audit, name the specific bug it would catch, and
confirm that bug is not something a small, focused code change could
introduce while every assertion in the test still passes. A test that
cannot name the bug it catches is a coverage-only test, whatever its
coverage number says.

## 6. Self-check before closing an audit

- [ ] Every test in the fast tier is hermetic, or explicitly excluded from
      that tier with a stated reason.
- [ ] The suite's current flake rate is known, or a concrete plan exists
      to find out; any test above the 1% action threshold has an explicit
      fix-now, quarantine, or delete decision, not silence.
- [ ] No finding treats a coverage percentage, by itself, as evidence of
      quality.
- [ ] Each finding names the specific test, the specific evidence (failure
      history, coupling, a named bug it would or would not catch), and one
      of the three flaky-test dispositions or an equivalent brittleness
      fix -- not a vague "tests need improvement."

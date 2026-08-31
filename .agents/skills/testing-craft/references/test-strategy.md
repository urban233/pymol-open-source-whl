# Test Strategy Reference

Distilled from *Software Engineering at Google*'s Testing Overview and
Larger Testing chapters, plus Google's public testing-infrastructure
research. Apply this to decide what needs a test, at what size and scope,
and which larger tests actually earn their cost for one change or one
system -- not to a single test's internals (see `writing-tests.md`) or to
an already-existing suite's health (see `test-suite-health.md`).

## Contents

1. Size and scope are two different questions
2. The target shape of a test suite
3. When a larger test earns its cost
4. The coverage-decision heuristic
5. Self-check before accepting a strategy

## 1. Size and scope are two different questions

- Size asks what resources a test may use: a **small** test runs in one
  process, with no network, no disk beyond scratch space, and no
  sleep-based waiting; a **medium** test may use multiple processes on one
  machine and blocking local calls; a **large** test may span multiple
  machines and is expected to be slower and less deterministic than the
  other two.
- Scope asks how much of the system a test exercises, independent of
  size: **narrow** scope covers one class or function in isolation;
  **medium** scope covers a handful of components interacting; **broad**
  scope covers the whole system, or a behavior that only exists once
  components are combined.
- Size and scope vary independently. A narrow-scope test can still be
  medium-sized if it genuinely needs a real browser or a real database; a
  broad-scope test can be small if every external dependency is a real,
  fast, in-process implementation. Do not assume "unit test" means
  "narrow and small" by definition -- check both axes.
- State the size and scope of each planned test group explicitly in a
  strategy, rather than leaving it implicit in a single word like "unit
  tests" or "integration tests."

## 2. The target shape of a test suite

- Aim for roughly 80% narrow/small tests, 15% medium tests, and 5%
  broad/large tests, by test count -- not by lines of test code or by
  runtime. This is a target shape for judging a whole suite, not a rule to
  defend line-by-line for one change's three new tests.
- Two shapes to name and avoid when they appear:
  - **Ice-cream cone**: mostly broad, slow, end-to-end tests with few
    narrow tests underneath. Symptom: most failures require reproducing a
    full environment to debug, and the suite is too slow to run on every
    change.
  - **Hourglass**: heavy at both narrow and broad, starved in the middle.
    Symptom: component-interaction bugs slip through because nothing
    exercises two or three real components together, only each alone or
    the whole system.
- A suite skewed toward more small tests than the target is not itself a
  problem; a suite skewed toward more large tests than the target usually
  is -- flag it explicitly rather than treating "more tests" as
  automatically healthier.

## 3. When a larger test earns its cost

Unit tests validate a function or class in isolation; they cannot catch
what only exists when real components combine: configuration bugs, load
behavior, an unfaithful test double, or an emergent interaction. A larger
test is worth its slower and less deterministic cost only when it targets
one of these specifically -- not as a blanket "more coverage is better"
addition. Before recommending one, name which gap it closes:

- **Functional**: one or more real binaries or services exercised through
  their published interface, to catch what an isolated unit test's mocks
  would hide.
- **Configuration or deployment smoke test**: confirms the system still
  starts and serves with its real configuration -- worth including
  whenever configuration has ever caused an incident, which in most
  systems it eventually has.
- **Performance, load, or stress**: capacity and latency under realistic
  concurrent traffic -- an emergent property no unit test can approximate.
- **Exploratory**: a human, or an agent acting as one, probes the system
  through its real interface for behavior nobody wrote a test for yet.
  Findings graduate into a permanent regression test; exploratory testing
  itself is not repeated as a suite.
- **Contract or integration**: verifies that two real components'
  interfaces still agree with each other, catching the case where each
  side's unit tests pass against a stale assumption about the other.

Do not add a larger test to "be thorough" without naming which of these
gaps it closes -- an unjustified large test buys slowness and flakiness
risk without a specific defect class in return.

## 4. The coverage-decision heuristic

If a strategy or a reviewer is willing to claim a behavior works, name the
specific test that proves it. This is the practical test for "have I
tested the right things": walk every claim in the change's stated
acceptance behavior and point to the test that would fail if that claim
became false. A behavior with no such test is untested, regardless of how
much unrelated code the suite covers. Line or branch coverage percentage
is a diagnostic for finding an obviously untested area, never a target to
hit for its own sake -- a change can reach 100% coverage while never
asserting the one behavior that actually matters, and a well-targeted
60%-covering suite can catch every regression that matters to the change.

## 5. Self-check before accepting a strategy

- [ ] Every planned test group states its size and scope explicitly, not
      just a label like "unit" or "integration."
- [ ] The overall shape is roughly 80/15/5 (narrow-small / medium /
      broad-large) by count, or the deviation is explained.
- [ ] Every larger test in the plan names the specific gap it closes
      (configuration, load, contract, exploratory finding, etc.), not
      "more coverage."
- [ ] Every claimed behavior in the change's acceptance criteria maps to a
      named test that would fail if the claim became false.
- [ ] Coverage percentage, if mentioned at all, is used to find a gap --
      never stated as the goal.

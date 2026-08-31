# Writing Tests Reference

Distilled from *Software Engineering at Google*'s Unit Testing and Test
Doubles chapters. Apply this to one test's internals once
`test-strategy.md` has settled what needs testing and at what size and
scope.

## Contents

1. DAMP, not DRY
2. Name and structure the behavior
3. The test-doubles priority ladder
4. The brittleness checklist
5. Self-check before committing a test

## 1. DAMP, not DRY

- Production code optimizes for DRY (don't repeat yourself); test code
  optimizes for DAMP (descriptive and meaningful phrases) instead. A test
  that reads top to bottom without the reader jumping to a shared helper
  to understand what is being verified is worth some duplication.
- Prefer a small helper that builds a test value with sensible defaults
  and named overrides for the fields the test actually cares about, over a
  shared constant whose meaning the reader must trace back to its
  definition.
- Shared setup is fine for constructing the object under test and its
  doubles; it becomes a liability once a test's correctness depends on a
  specific value buried in that shared setup that the test itself never
  states.

## 2. Name and structure the behavior

- Name a test after the behavior it verifies, not the method it calls:
  `shouldRejectWithdrawalWhenBalanceIsEmpty`, not `testWithdraw`.
- Structure a test's body as Given (set up the state), When (take the
  action), Then (assert the result) -- in that order, with nothing
  extraneous in any of the three parts.
- One behavior per test. A test that verifies two unrelated behaviors
  doubles its own failure ambiguity: a failure no longer tells you which
  behavior broke.
- No control flow (loops, conditionals) inside a test body. A test has no
  tests of its own, so it must be correct on inspection; a conditional
  inside a test hides a bug the test itself could contain.

## 3. The test-doubles priority ladder

Prefer, in this order, the option with the highest fidelity -- how closely
the test's behavior matches production's -- that is still practical:

1. **Real implementation.** Skip only when it is slow, non-deterministic,
   or genuinely hard to construct (for example, it dials out to another
   service).
2. **Fake.** A lightweight implementation that behaves like the real one
   (an in-memory database, for example), ideally maintained by the real
   implementation's own owner and covered by its own contract tests so it
   cannot silently drift from what it stands in for.
3. **Mock or stub, as a last resort.** Its behavior is hand-specified
   inside the test, so it can diverge from the real implementation's
   actual behavior; reserve it for a path that is hard to trigger any
   other way, such as a specific timeout or error condition.

Avoid pure interaction testing -- asserting that a function was called,
rather than what the system produced -- except as a last resort for a
function whose only observable effect is the call itself (for example,
verifying a cache is not hit excessively). An interaction test that could
be replaced by a state assertion should be. Do not mock a type its own
owner has marked as not meant to be mocked, or a type you do not own at
all -- a test double for code you do not control encodes an assumption
about its contract with no mechanism to warn you when that contract
changes.

## 4. The brittleness checklist

A brittle test fails from an unrelated change to production code that
introduced no real bug -- it costs the whole team's future changes to that
code, not just its author. Before accepting a test, check:

- It exercises the system through the same interface real callers use,
  not through an internal method or field only the test can reach.
- It asserts on state -- what the system produced -- rather than on which
  internal calls were made to produce it, wherever a state assertion is
  possible.
- It contains only the setup and assertions needed for its one behavior;
  deleting any remaining line would make the test fail to compile or fail
  to catch its intended bug.
- A failure message states what was expected and what was actually
  produced, not just "assertion failed" -- write a plain expected/actual
  message when the framework's default is not it.

## 5. Self-check before committing a test

- [ ] The test's name describes the behavior it verifies, not the method
      under test.
- [ ] The test follows Given/When/Then with no unrelated setup and no
      control flow in its body.
- [ ] Each dependency uses the highest-fidelity option available (real,
      then fake, then mock/stub), and the choice is not simply "whatever
      was fastest to write."
- [ ] The test exercises the system through its real interface and
      asserts on state, not on internal calls, unless an interaction
      assertion is genuinely the only option.
- [ ] A failing run of this test would tell a future engineer, without
      reading the test's own source, what broke.

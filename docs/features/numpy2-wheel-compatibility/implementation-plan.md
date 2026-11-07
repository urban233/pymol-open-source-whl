# NumPy 2 wheel compatibility

Status: Implemented and independently reviewed; full wheel CI pending.
Base commit: 5efcb396b01b24c2c679fc02e767c4df760ce6b4.
Owner and release approver: repository owner.
Execution: delegated builder, followed by a fresh read-only reviewer.

## Scope and acceptance

Make every release wheel exercise its native NumPy boundary before publication.
Keep the existing NumPy 2 runtime requirement and current twenty-wheel platform
and Python matrix. Do not introduce NumPy 1 variants, new dependencies, unrelated
refactors, GUI tests, or changes to the untracked `.codev/` and `docs/codev/` work.

Acceptance requires unconditional, deterministic installed-wheel tests for
coordinate extraction, array modification and coordinate loading, and a volume
array round trip. Missing NumPy support or a broken ABI must fail, never skip.
The current binary must demonstrate a regression failure before the CMake fix;
the rebuilt wheel must pass the same test. Ordinary default wheel tests must
not depend on an external structure server.

## Implementation

1. Add the NumPy wheel smoke tests outside the capability-gated undo directory.
   Use existing public PyMOL and chempy APIs, explicit floating point arrays,
   small local structures, and isolated PyMOL state. Replace the default remote
   fetch check with a deterministic local import/export check.
2. Exercise the installed artifact at the declared NumPy 2 floor compatible
   with each Python and at the current NumPy 2 version. Verify version/platform
   diagnostics and retain separate JUnit reports per wheel and NumPy version.
   Keep the existing publication dependency chain and wheel coverage validator.
   Confirm actual supported NumPy versions before choosing Python 3.14's floor.
3. Make CMake discover NumPy from the selected Python build environment and
   put those headers ahead of `pre-built/include`. Do not change the existing
   deprecation API definition or replace the vendored header directory.
4. Check the published version, then prepare the next unused patch release in
   `pyproject.toml` and refresh only the necessary project entry in `uv.lock`.
   Document the wheel gate and compatibility policy in the existing README.

Expected scope: `tests/`, `scripts/`, `CMakeLists.txt`, `pyproject.toml`,
`uv.lock`, `.github/workflows/wheels.yaml`, and the relevant README paragraphs.

## Validation and release

Run a real regression test against the old extension with NumPy 2, then rebuild
and install the fixed Windows CPython 3.12 wheel in an isolated environment.
Run the wheel suite against the supported minimum and current NumPy 2, run
targeted lint/format checks, inspect the complete diff, and obtain independent
review. Cross-platform acceptance requires the release wheel workflow for all
twenty artifacts; local Windows results alone cannot satisfy it.

Stop for a material source incompatibility, conflicting release version,
unavailable essential validation, or edits colliding with concurrent work.
Do not merge, tag, or publish an unreviewed or incompletely tested release.
Prepare the concrete candidate and review evidence before a final release
authorization step, if one is required.

## Evidence

- Released Windows CPython 3.12 wheel `3.2.0.2` with NumPy `2.2.0`:
  all three new native NumPy cases failed with the NumPy 1 ABI error;
  deterministic local import/export passed.
- Rebuilt Windows CPython 3.12 wheel `3.2.0.3`: the exact CI runner, invoked
  outside the source directory, passed with NumPy `2.2.0` and `2.5.4`.
  Each profile reported 20 passed and 10 capability-gated undo skips;
  all three mandatory NumPy cases passed, and `pip check` passed.
- Targeted Ruff lint/format, `git diff --check`, and `uv lock --check --offline`
  passed. Independent read-only review found no actionable source defects.
- The local native build reused installed vcpkg libraries with a command-line
  `VCPKG_MANIFEST_INSTALL=OFF` override because the old local vcpkg checkout
  could not resolve the manifest. The release workflow must validate clean
  native dependency provisioning and every platform before publication.
- PyPI reports `3.2.0.2` as current; `3.2.0.3` is unused. The current production
  commit `e84604a3` has the same tree as the implementation base above.

# L-01 baseline and candidate proof

**Task:** `test-suite-l01-baseline`
**Base snapshot:** `1c3f3f309c3f00468cb15e97488bff2c42b97bba`
**Observed:** 2026-08-25 on Windows 11 x64

## Result summary

The requested baseline could not be collected or executed. The repository
global Python has no `pytest` or `pymol`; the repository `.venv` exists but its
Python launcher points to the missing executable
`C:\MyPrograms\Installed\MyPython\Python311\python.exe`. No test result,
duration, skip report, or collected pytest node ID is therefore claimed below.
Static source and data inspection was completed and is explicitly separated
from runtime evidence.

## Environment evidence

Commands run from the repository root:

```text
python --version
Python 3.12.10

python -m pytest --version
C:\Program Files\Python312\python.exe: No module named pytest

python -c "import sys, platform; print('executable=',sys.executable); print('version=',sys.version); print('platform=',platform.platform()); import pymol; print('pymol=',pymol.__file__)"
executable= C:\Program Files\Python312\python.exe
version= 3.12.10 ... [MSC v.1943 64 bit (AMD64)]
platform= Windows-11-10.0.26200-SP0
ModuleNotFoundError: No module named 'pymol'

.\.venv\Scripts\python.exe --version
No Python at '"C:\MyPrograms\Installed\MyPython\Python311\python.exe'

py --version
Python 3.12.10
```

The available agent Python at
`C:\Users\manfred\AppData\Local\hermes\hermes-agent\venv\Scripts\python.exe`
is Python 3.11.12, but it also reports `No module named pytest` and
`ModuleNotFoundError: No module named 'pymol'`. There is no importable PyMOL
path in the inspected environments. No environment creation or wheel install
was attempted because this L-01 scope is read-only and the repository has no
available local wheel/native environment to use.

## Root collection and execution

Requested collection command:

```text
python -m pytest --collect-only -q tests
C:\Program Files\Python312\python.exe: No module named pytest
exit=1
```

Collected node IDs/count: unavailable because pytest did not start.

Requested root execution was also attempted:

```text
python -m pytest -q tests
C:\Program Files\Python312\python.exe: No module named pytest
exit=1
```

This is an environment/collection prerequisite failure, not a test execution
failure. No duration or skip data was produced.

## Nominated legacy collection and execution

The repository separates legacy pytest configuration in `testing/pytest.ini`
(`--import-mode=importlib`, `testpaths = tests`, and `python_files = *.py`).
The nominated collection command used that configuration explicitly:

```text
python -m pytest --collect-only -q -c testing/pytest.ini testing/tests/api/test_commanding.py testing/tests/api/test_importing.py testing/tests/api/test_exporting.py testing/tests/api/test_selecting.py testing/tests/api/test_fitting.py testing/tests/undo/api/test_undo_creating.py testing/tests/undo/api/test_undo_importing.py
C:\Program Files\Python312\python.exe: No module named pytest
exit=1
```

The corresponding execution command was attempted:

```text
python -m pytest -q -c testing/pytest.ini testing/tests/api/test_commanding.py testing/tests/api/test_importing.py testing/tests/api/test_exporting.py testing/tests/api/test_selecting.py testing/tests/api/test_fitting.py testing/tests/undo/api/test_undo_creating.py testing/tests/undo/api/test_undo_importing.py
C:\Program Files\Python312\python.exe: No module named pytest
exit=1
```

The legacy runner probe was also unavailable:

```text
pymol testing/testing.py --run testing/tests/api/test_commanding.py
The term 'pymol' is not recognized ...
exit=1
```

Thus these are collection-start failures, not failures of the nominated test
cases. Runtime node IDs, expanded parametrized IDs, durations, and skips are
unavailable.

## Candidate inventory (static inspection only)

All listed fixed data files exist under `testing/data/`. The expected marker
is a recommendation for L-02/L-03, not an observed marker: the nominated
legacy files currently have no design taxonomy markers.

| Candidate | Static inventory and state assumptions | Capability / future marker recommendation |
|---|---|---|
| `testing/tests/api/test_commanding.py` | 13 test functions. Defines commands through `cmd.new_command` and invokes `cmd.do`; no repository data, network, GUI, subprocess, or output files. The legacy API autouse fixture calls `cmd.reinitialize()` before each test. One test is explicitly skipped; one is Python 3.11+ conditional. | `medium` because it uses the in-process PyMOL API. No capability marker observed. Candidate for deterministic port, subject to confirming command-global state cleanup. |
| `testing/tests/api/test_importing.py` | 2 tests. Loads fixed `115d.bcif.gz`; queries BCIF arrays. Legacy API fixture resets before each test. Version decorators require PyMOL >=3.0. | `medium`; no network/GUI/optional dependency marker observed. Preserve a named version/capability skip if required by the supported wheel. |
| `testing/tests/api/test_exporting.py` | 2 tests. Creates `ala`/`gly`, saves BCIF to `tempfile.NamedTemporaryFile(delete=False)`, reloads, and deletes the file in `finally`. Legacy API fixture resets before each test. Version decorators require PyMOL >=3.2. | `medium`; no network/GUI/optional dependency marker observed. Port should replace process-global temporary-file behavior with the approved temporary-path fixture. |
| `testing/tests/api/test_selecting.py` | 4 tests. Uses fixed `1pup.cif` and `1ehz-5.pdb`, plus a pseudoatom. One test redundantly calls `cmd.reinitialize()`; the API fixture also resets. | `medium`; no network/GUI/optional dependency marker observed. Fixed-data resolution is suitable for a shared fixture. |
| `testing/tests/api/test_fitting.py` | 10 tests. Uses fixed `1oky-frag.pdb`, `1t46-frag.pdb`, `1rx1.pdb`, and `1bna.cif`; uses NumPy coordinate comparison and `cmd.usalign`. Legacy API fixture resets before each test. | `medium`; no network/GUI/optional dependency marker observed. Confirm platform/version stability of numerical thresholds before admission. |
| `testing/tests/undo/api/test_undo_creating.py` | 14 test functions, including 3 `pass` placeholders. Uses generated fragments, pseudoatoms, maps, surfaces, groups, and undo/redo. Undo fixture resets, requires `multi_undo`, enables undo before each test, and disables it afterward. `test_undo_isolevel` additionally inspects rendered colors through `ambientOnly`/`imageHasColor`. | Generally `medium` with a `multi_undo` capability skip. `test_undo_isolevel` cannot be safely classified from source alone because its rendering capability requirement is implicit rather than declared; keep it out of a default candidate until runtime evidence. |
| `testing/tests/undo/api/test_undo_importing.py` | 2 test functions; `test_undo_load_traj` expands to 3 parameter sets. Raw load uses fixed `sampletrajectory.pdb`; trajectory cases use fixed `.dcd`, `.crd`, `.xtc` with `.pdb`/`.gro` topology and exercise multiple state/interval ranges. Undo fixture resets and requires/enables `multi_undo`. | `medium` plus `multi_undo` capability. The raw-load case matches the design's initial candidate. The trajectory permutations are explicitly follow-up work in the design and should not be admitted without separate runtime evidence. |

Static AST inventory found 47 declared test functions across the seven files,
plus three parametrized trajectory cases (50 logical function/parameter
instances before skip expansion). This is not a pytest collection count.

## Root and legacy assumptions observed

The root suite currently contains `tests/test_shortcut.py`,
`tests/test_pymol.py`, and root undo tests. Source/design inspection identifies
the shortcut tests as pure Python, the root fetch test as network and
source-tree-output dependent, and root undo tests as stateful. These are design
classifications, not collected marker evidence; the current root suite has no
validated baseline marker inventory.

The legacy API fixture resets PyMOL before each test. The legacy undo fixture
resets state, skips when `multi_undo` is unavailable, enables undo before each
test, and disables it afterward. The legacy runner additionally contains
working-directory and capability handling, but its `pymol` executable was not
available. No nominated candidate was shown by runtime evidence to be
deterministic, nondeterministic, skipped, or platform-specific.

## Validation and limitations

The complete report was inspected after creation. `git diff --check` was run
and passed. No formatter, static checker, pytest collection, or test execution
could provide additional evidence because pytest/PyMOL are unavailable in the
current usable interpreters.

Recommendations for the next increment are therefore provisional: establish a
valid installed-wheel or native environment first; collect exact pytest node
IDs and expanded parametrized IDs; run the nominated files with the separate
legacy configuration; then decide whether the implicit rendering case and
trajectory permutations satisfy the accepted deterministic candidate policy.

import pathlib
import subprocess

import const


def run_pytest_suite():
  """Runs the pytest suite."""
  # <editor-fold desc="OS specific code">
  if const.WIN32:
    tmp_pytest_exe_filepath = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/Scripts/pytest")
  elif const.__APPLE__ or const.__linux__:
    tmp_pytest_exe_filepath = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/bin/pytest")
  else:
    exit(const.invalid_platform())
  # </editor-fold>
  subprocess.run(
    [tmp_pytest_exe_filepath],
    cwd=pathlib.Path(const.PROJECT_ROOT_DIR / "tests")
  )

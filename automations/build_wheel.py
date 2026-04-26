import pathlib
import subprocess

import const
import utils


def run_wheel_build():
  """Runs the wheel file build proces."""
  # <editor-fold desc="OS specific code">
  if const.WIN32:
    tmp_python_exe_filepath = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/Scripts/python")
  elif const.__APPLE__ or const.__linux__:
    tmp_python_exe_filepath = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/bin/python")
  else:
    exit(const.invalid_platform())
  # </editor-fold>
  utils.copy_pymol_sources()
  subprocess.run(
    [tmp_python_exe_filepath, "setup.py", "bdist_wheel"],
    cwd=pathlib.Path(const.PROJECT_ROOT_DIR)
  )

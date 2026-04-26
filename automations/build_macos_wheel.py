"""
#A* -------------------------------------------------------------------
#B* This file contains source code for running automation tasks related
#-* to the build process of the PyMOL computer program
#C* Copyright 2025 by Martin Urban.
#D* -------------------------------------------------------------------
#E* It is unlawful to modify or remove this copyright notice.
#F* -------------------------------------------------------------------
#G* Please see the accompanying LICENSE file for further information.
#H* -------------------------------------------------------------------
#I* Additional authors of this source file include:
#-*
#-*
#-*
#Z* -------------------------------------------------------------------
"""
import sys
import pathlib
import shutil
import subprocess

import const
import utils


def build_wheel() -> None:
  """Builds the wheel file for the python PyMOL package."""
  tmp_python_version = f"{sys.version_info.major}.{sys.version_info.minor}"
  tmp_shared_suffix = f".cpython-{tmp_python_version.replace('.', '')}-darwin.so"
  if not pathlib.Path(const.PROJECT_ROOT_DIR / "src/python/pymol").exists():
    utils.copy_pymol_sources()
    _CMD_FROM_BUILD_DIR = pathlib.Path(const.PROJECT_ROOT_DIR / "cmake-build-release" / f"_cmd{tmp_shared_suffix}")
    _CMD_FROM_PRE_BUILT_DIR = pathlib.Path(const.PROJECT_ROOT_DIR / "pre-built" / f"_cmd{tmp_shared_suffix}")
    if _CMD_FROM_BUILD_DIR.exists():
      shutil.copy(
        _CMD_FROM_BUILD_DIR,
        pathlib.Path(const.PROJECT_ROOT_DIR / "src/python/pymol" / f"_cmd{tmp_shared_suffix}")
      )
    else:
      shutil.copy(
        _CMD_FROM_PRE_BUILT_DIR,
        pathlib.Path(const.PROJECT_ROOT_DIR / "src/python/pymol" / f"_cmd{tmp_shared_suffix}")
      )
  else:
    print("The src/python/pymol directory already exists, that might mean a self compiled _cmd module was built.")
  subprocess.run(
    [const.PYTHON_EXECUTABLE, 'setup.py', 'sdist', 'bdist_wheel'],
    stdout=sys.stdout, stderr=sys.stderr, text=True
  )
  shutil.rmtree(pathlib.Path(const.PROJECT_ROOT_DIR / "src"))


def clean_install() -> None:
  """Cleans the CMake build directory and then runs the complete build process."""
  tmp_pip_executable = pathlib.Path(const.PROJECT_ROOT_DIR / ".venv/bin", "pip")
  tmp_build_dir = pathlib.Path(const.PROJECT_ROOT_DIR / "cmake-build-setup_py")
  utils.copy_pymol_sources()

  if tmp_build_dir.exists():
    shutil.rmtree(tmp_build_dir)
  subprocess.run(
    [tmp_pip_executable, 'install', '.'],
    stdout=sys.stdout, stderr=sys.stderr, text=True
  )

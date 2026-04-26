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
import pathlib
import shutil
import subprocess
import platform

# TODO: Change this into something better
try:
  from . import const  # This is needed by cibuildwheel
except ImportError:
  import const  # This is needed by my_automator.py


def copy_pymol_sources() -> None:
  """Copies the pymol python sources from the vendor directory."""
  tmp_src_path = pathlib.Path(const.PROJECT_ROOT_DIR / "src/python")
  tmp_pymol_python_src_path = pathlib.Path(const.PROJECT_ROOT_DIR / "vendor/pymol-open-source/modules")
  if not tmp_src_path.exists():
    print("Copying the pymol python sources ...")
    tmp_src_path.mkdir(parents=True)
    shutil.copytree(
      tmp_pymol_python_src_path,
      tmp_src_path,
      dirs_exist_ok=True
    )


def get_mac_architecture() -> str:
  """Gets the current running architecture for macOS."""
  try:
    # Get the hardware architecture using sysctl
    arch = subprocess.check_output(
      ['sysctl', '-n', 'hw.machine'],
      stderr=subprocess.DEVNULL
    ).decode().strip()
    return arch
  except subprocess.CalledProcessError:
    # Fallback to platform.machine() if sysctl isn't available (unlikely on macOS)
    return platform.machine()


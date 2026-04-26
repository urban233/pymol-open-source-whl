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
import subprocess

import const
import utils


def build_cmd_module():
  """Function that compiles the _cmd module from source."""
  utils.copy_pymol_sources()
  subprocess.run(
    [
      const.PYTHON_EXECUTABLE,
      pathlib.Path(const.PROJECT_ROOT_DIR / "setup.py"),
      "build_ext"
    ]
  )

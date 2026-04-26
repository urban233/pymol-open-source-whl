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
import platform
import sys
import pathlib

PROJECT_ROOT_DIR = pathlib.Path(__file__).parent.parent
PYTHON_EXECUTABLE = sys.executable


def invalid_platform() -> int:
  """Function that reports that the platform is invalid and returns 1.

  Note:
    This function does not automatically crash the program because this
    creates problems with pythons static type checker. Therefore, the
    function needs to be nested inside exit().
  """
  print(f"Invalid platform: {platform.system()}")
  return 1


# <editor-fold desc="Check running OS">
if platform.system() == "Windows":
  WIN32 = True
  __APPLE__ = False
  __linux__ = False
elif platform.system() == "Darwin":
  WIN32 = False
  __APPLE__ = True
  __linux__ = False
elif platform.system() == "Linux":
  WIN32 = False
  __APPLE__ = False
  __linux__ = True
else:
  exit(invalid_platform())
# </editor-fold>

"""
#A* -------------------------------------------------------------------
#B* This file contains source code for running a simple example
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

import pymol
from pymol import cmd

_FILEPATH = pathlib.Path(__file__).parent


# Use this only for internal testing!
# def test_startup():
#   """Tests if PyMOL can be launched.
#
#   Note:
#     Do NOT use this in any CI/CD workflow
#   """
#   pymol.launch()


def test_fetch() -> None:
  """Tests if the fetch command works correctly."""
  cmd.fetch("3bmp")
  cmd.save(str(pathlib.Path(_FILEPATH / "3bmp.cif")), "3bmp")
  assert pathlib.Path(_FILEPATH / "3bmp.cif").exists() is True
  assert cmd.get_model("3bmp").atom[0].model == "3bmp"

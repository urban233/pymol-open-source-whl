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

from pathlib import Path

from pymol2 import PyMOL


def test_local_import_export(tmp_path: Path) -> None:
    """Exercise installed-wheel import/export without a remote server."""
    with PyMOL() as session:
        cmd = session.cmd
        cmd.pseudoatom("original", name="CA", elem="C", pos=[1.0, 2.0, 3.0])
        structure = tmp_path / "structure.pdb"
        cmd.save(str(structure), "original")
        assert structure.is_file()

        cmd.delete("original")
        cmd.load(str(structure), "restored")
        assert cmd.count_atoms("restored") == 1
        assert cmd.get_model("restored").atom[0].name == "CA"
        assert cmd.get_atom_coords("restored") == [1.0, 2.0, 3.0]

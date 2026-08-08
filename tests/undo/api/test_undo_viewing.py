import pytest

from chempy import cpv
from pymol import cmd


def test_undo_clean():
    cmd.fragment("his")
    original_coords = cmd.get_model("his").get_coord_list()
    cmd.clean("his")
    cleaned_coords = cmd.get_model("his").get_coord_list()
    assert len(original_coords) == len(cleaned_coords)
    assert original_coords != cleaned_coords
    cmd.undo()
    assert original_coords == cmd.get_model("his").get_coord_list()
    cmd.redo()
    assert original_coords != cmd.get_model("his").get_coord_list()


def test_undo_origin():
    cmd.pseudoatom("m1")
    cmd.pseudoatom("m2")
    cmd.pseudoatom("m3", pos=[1, 0, 0])
    cmd.origin("m3")
    cmd.rotate("y", 90, "m1")
    cmd.origin(position=[-1, 0, 0])
    cmd.undo()
    cmd.rotate("y", 90, "m2")
    coords = []
    cmd.iterate_state(1, "m1 m2", "coords.append([x,y,z])", space=locals())
    assert cpv.distance(*coords) == pytest.approx(0)

    cmd.delete("*")
    cmd.pseudoatom("m1")
    cmd.pseudoatom("m2")
    cmd.pseudoatom("m3", pos=[1, 0, 0])
    cmd.origin("m3")
    cmd.rotate("y", 90, "m1")
    cmd.origin(position=[-1, 0, 0])
    cmd.undo()
    cmd.redo()
    cmd.rotate("y", 90, "m2")
    coords = []
    cmd.iterate_state(1, "m1 m2", "coords.append([x,y,z])", space=locals())
    assert cpv.distance(*coords) == pytest.approx(2 * 2**0.5)

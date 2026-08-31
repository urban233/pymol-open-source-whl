import pytest

from pymol import cmd

pytestmark = pytest.mark.medium


def test_undo_protect():
    cmd.pseudoatom("m1", pos=[0.0, 0.0, 0.0])
    cmd.pseudoatom("m1", pos=[1.0, 0.0, 0.0])
    cmd.protect("m1`1")
    cmd.undo()
    cmd.translate([0.0, 0.0, 1.0])
    assert [0.0, 0.0, 1.0] == cmd.get_atom_coords("m1`1")
    assert [1.0, 0.0, 1.0] == cmd.get_atom_coords("m1`2")
    cmd.protect("m1`1")
    cmd.undo()
    cmd.redo()
    cmd.translate([0.0, 1.0, 0.0])
    assert [0.0, 0.0, 1.0] == cmd.get_atom_coords("m1`1")
    assert [1.0, 1.0, 1.0] == cmd.get_atom_coords("m1`2")


def test_undo_deprotect():
    cmd.pseudoatom("m1", pos=[0.0, 0.0, 0.0])
    cmd.pseudoatom("m1", pos=[1.0, 0.0, 0.0])
    cmd.protect("m1`1")
    cmd.deprotect()
    cmd.undo()
    cmd.translate([0.0, 0.0, 1.0])
    assert [0.0, 0.0, 0.0] == cmd.get_atom_coords("m1`1")
    assert [1.0, 0.0, 1.0] == cmd.get_atom_coords("m1`2")
    cmd.protect("m1`1")
    cmd.deprotect()
    cmd.undo()
    cmd.redo()
    cmd.translate([0.0, 1.0, 0.0])
    assert [0.0, 1.0, 0.0] == cmd.get_atom_coords("m1`1")
    assert [1.0, 1.0, 1.0] == cmd.get_atom_coords("m1`2")


def _assert_array_equal(first, second, not_equal=False):
    import numpy

    first = numpy.asarray(first)
    second = numpy.asarray(second)
    assert first.shape == second.shape
    assert not_equal != numpy.allclose(first, second, 0, 0)


def test_undo_update():
    cmd.fragment("gly", "m1")
    cmd.create("m1", "m1", 1, 2)
    cmd.create("m1", "m1", 1, 3)
    cmd.copy("m2", "m1")
    cmd.rotate("x", 90, "(m2)", state=0)
    coordinate_set = cmd.get_coordset
    first = coordinate_set("m1", 1)
    second = coordinate_set("m2", 1)
    _assert_array_equal(second, coordinate_set("m2", 3))
    _assert_array_equal(first, coordinate_set("m2", 3), not_equal=True)
    cmd.update("m2", "m1", 3, 2)
    _assert_array_equal(first, coordinate_set("m2", 3))
    _assert_array_equal(second, coordinate_set("m2", 3), not_equal=True)
    cmd.undo()
    _assert_array_equal(first, coordinate_set("m2", 3), not_equal=True)
    _assert_array_equal(second, coordinate_set("m2", 3))
    cmd.redo()
    _assert_array_equal(first, coordinate_set("m2", 3))
    _assert_array_equal(second, coordinate_set("m2", 3), not_equal=True)


def test_undo_fit():
    cmd.fragment("gly", "m1")
    cmd.create("m2", "m1")
    cmd.rotate("y", "90", "m2")
    assert cmd.rms_cur("m1", "m2") != pytest.approx(0.0)
    cmd.fit("m1", "m2")
    assert cmd.rms_cur("m1", "m2") == pytest.approx(0.0)
    cmd.undo()
    assert cmd.rms_cur("m1", "m2") != pytest.approx(0.0)
    cmd.redo()
    assert cmd.rms_cur("m1", "m2") == pytest.approx(0.0)

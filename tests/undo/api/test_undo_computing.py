import pytest

from pymol import cmd

pytestmark = pytest.mark.medium


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

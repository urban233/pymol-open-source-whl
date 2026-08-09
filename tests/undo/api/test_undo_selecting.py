from pymol import cmd
from pymol.constants import ALL_STATES


def _undo_selection(
    selection,
    current_atom_count,
    current_names,
    original_atom_count,
    original_names,
):
    assert current_atom_count == cmd.count_atoms(selection)
    assert current_names == cmd.get_names("selections")
    cmd.undo()
    assert original_names == cmd.get_names("selections")
    cmd.redo()
    assert current_atom_count == cmd.count_atoms(selection)
    assert current_names == cmd.get_names("selections")


def test_undo_select():
    cmd.fragment("gly", "m1")
    atom_count = 2
    cmd.select("elem C")
    _undo_selection("sele", atom_count, ["sele"], 0, [])
    assert cmd.get_setting_int("sel_counter") == 0
    cmd.delete("sele")


def test_undo_auto_number_select():
    cmd.fragment("gly", "m1")
    atom_count = 2
    cmd.set("auto_number_selections", 1)
    cmd.set("sel_counter", 3)

    cmd.select("elem C")
    _undo_selection("sel04", atom_count, ["sel04"], 0, [])
    assert cmd.get_setting_int("sel_counter") == 4

    cmd.select(None, "elem C")
    _undo_selection("sel05", atom_count, ["sel04", "sel05"], 0, ["sel04"])
    assert cmd.get_setting_int("sel_counter") == 5
    cmd.delete("sel*")

    cmd.set("auto_number_selections", 0)
    cmd.select(None, "elem C")
    _undo_selection("sel06", atom_count, ["sel06"], 0, [])
    assert cmd.get_setting_int("sel_counter") == 6
    cmd.delete("sel*")


def _undo_assert_selections(
    target_selection,
    previous_atom_count,
    previous_all_names,
    previous_enabled_names,
    current_atom_count,
    current_all_names,
    current_enabled_names,
    state=ALL_STATES,
):
    assert current_atom_count == cmd.count_atoms(target_selection, state=state)
    assert current_all_names == cmd.get_names("selections", enabled_only=0)
    assert current_enabled_names == cmd.get_names("selections", enabled_only=1)
    cmd.undo()
    assert previous_atom_count == cmd.count_atoms(target_selection, state=state)
    assert previous_all_names == cmd.get_names("selections", enabled_only=0)
    assert previous_enabled_names == cmd.get_names("selections", enabled_only=1)
    cmd.redo()
    assert current_atom_count == cmd.count_atoms(target_selection, state=state)
    assert current_all_names == cmd.get_names("selections", enabled_only=0)
    assert current_enabled_names == cmd.get_names("selections", enabled_only=1)


def test_undo_select_merge():
    cmd.fragment("gly", "m1")
    atom_count = 2
    cmd.select("foo", "elem C", 0, merge=1)
    _undo_assert_selections("?foo", 0, [], [], atom_count, ["foo"], [])

    cmd.select("foo", "elem N", 1)
    _undo_assert_selections("?foo", atom_count, ["foo"], [], 1, ["foo"], ["foo"])

    cmd.select("foo", "elem C", -1, merge=1)
    _undo_assert_selections(
        "?foo", atom_count - 1, ["foo"], ["foo"], atom_count + 1, ["foo"], ["foo"]
    )

    cmd.select("foo", "elem O", -1, merge=2)
    assert cmd.count_atoms("foo") == atom_count + 2
    assert cmd.get_names("selections", enabled_only=0) == ["foo"]
    assert cmd.get_names("selections", enabled_only=1) == ["foo"]

    cmd.select("foo", "elem N", 1)
    cmd.select("foo", "elem N", 0)
    _undo_assert_selections("?foo", 1, ["foo"], ["foo"], 1, ["foo"], [])

    cmd.select("foo", "elem C", -1, merge=1)
    _undo_assert_selections("?foo", atom_count - 1, ["foo"], [], atom_count + 1, ["foo"], [])

    cmd.select("foo", "elem O", -1, merge=2)
    _undo_assert_selections("?foo", atom_count + 1, ["foo"], [], atom_count - 1, ["foo"], [])

import pytest

from pymol import cmd
import pymol


has_multi_undo = "multi_undo" in pymol.get_capabilities()


@pytest.fixture(autouse=True)
def setup():
    if not has_multi_undo:
        pytest.skip("multi-state undo is unavailable")
    cmd.reinitialize()
    yield


@pytest.fixture(autouse=True)
def undo_enable(setup):
    cmd.undo_enable()
    yield
    cmd.undo_disable()

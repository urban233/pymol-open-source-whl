import pytest


@pytest.fixture(autouse=True)
def setup():
    from pymol import cmd, get_capabilities

    if "multi_undo" not in get_capabilities():
        pytest.skip("multi-state undo is unavailable")
    cmd.reinitialize()
    try:
        yield
    finally:
        cmd.reinitialize()


@pytest.fixture(autouse=True)
def undo_enable(setup):
    from pymol import cmd

    cmd.undo_enable()
    try:
        yield
    finally:
        cmd.undo_disable()


@pytest.fixture
def output_path(tmp_path):
    return tmp_path


@pytest.fixture
def data_path():
    from pathlib import Path

    return Path(__file__).parents[1] / "data"

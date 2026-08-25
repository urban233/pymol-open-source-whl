"""Shared fixtures for the multi-state undo tests."""

import pytest


@pytest.fixture(autouse=True)
def setup():
    """Reset PyMOL before and after each undo test.

    Yields:
        Control to the test after the initial reset.
    """
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
    """Enable undo tracking for a test and disable it afterward.

    Args:
        setup: The fixture that resets PyMOL around the test.

    Yields:
        Control to the test with undo tracking enabled.
    """
    from pymol import cmd

    cmd.undo_enable()
    try:
        yield
    finally:
        cmd.undo_disable()


@pytest.fixture
def output_path(tmp_path):
    """Provide a temporary directory for test output.

    Args:
        tmp_path: Pytest's temporary directory fixture.

    Returns:
        The temporary directory path.
    """
    return tmp_path


@pytest.fixture
def data_path():
    """Provide the undo test data directory.

    Returns:
        The path containing the undo test data files.
    """
    from pathlib import Path

    return Path(__file__).parents[1] / "data"

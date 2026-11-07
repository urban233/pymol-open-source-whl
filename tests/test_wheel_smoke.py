"""Mandatory native NumPy checks for every installed release wheel."""

import numpy as np
import pytest
from chempy.brick import Brick
from pymol2 import PyMOL


@pytest.fixture
def molecular_session():
    with PyMOL() as session:
        yield session.cmd


@pytest.mark.parametrize("dtype", [np.float32, np.float64], ids=["float32", "float64"])
def test_numpy_coordinates(molecular_session, dtype):
    cmd = molecular_session
    original = np.array([[1.0, 2.0, 3.0], [-4.0, 5.0, -6.0]], dtype=dtype)
    for position in original:
        cmd.pseudoatom("atoms", pos=position.tolist())

    coordinates = cmd.get_coords("atoms")
    assert isinstance(coordinates, np.ndarray)
    assert coordinates.shape == (2, 3)
    np.testing.assert_allclose(coordinates, original)
    np.testing.assert_allclose(cmd.get_coordset("atoms"), original)

    # A strided array checks the native input boundary as well as extraction.
    changed = (coordinates.astype(dtype) + 0.5)[:, ::-1]
    cmd.load_coords(changed, "atoms")
    np.testing.assert_allclose(cmd.get_coords("atoms"), changed)
    np.testing.assert_allclose(cmd.get_coordset("atoms"), changed)


def test_numpy_volume(molecular_session):
    cmd = molecular_session
    values = np.arange(24, dtype=np.float64).reshape(2, 3, 4) / 4.0
    brick = Brick.from_numpy(values, grid=(0.5, 1.0, 1.5))
    cmd.load_brick(brick, "density")

    result = cmd.get_volume_field("density", copy=1)
    assert isinstance(result, np.ndarray)
    assert result.shape == values.shape
    np.testing.assert_allclose(result, values)

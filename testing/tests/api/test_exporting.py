import os
from pymol import cmd
from pymol import test_utils
import tempfile


@test_utils.requires_version("3.2")
def test_bcif_export():
    """Test BCIF export and round-trip"""
    # Create a simple structure
    cmd.fragment("ala")
    orig_count = cmd.count_atoms("ala")
    assert orig_count == 10

    # Export to BCIF
    with tempfile.NamedTemporaryFile(suffix='.bcif', delete=False) as f:
        bcif_file = f.name

    try:
        cmd.save(bcif_file, "ala")
        assert os.path.exists(bcif_file)
        assert os.path.getsize(bcif_file) > 0

        # Load back and verify
        cmd.delete("all")
        cmd.load(bcif_file, "test_loaded")
        loaded_count = cmd.count_atoms("test_loaded")
        assert loaded_count == orig_count, f"Atom count mismatch: {loaded_count} != {orig_count}"
    finally:
        if os.path.exists(bcif_file):
            os.unlink(bcif_file)


@test_utils.requires_version("3.2")
def test_bcif_export_multi_object():
    """Test BCIF export with multiple objects"""
    cmd.fragment("ala")
    cmd.fragment("gly")
    ala_count = cmd.count_atoms("ala")
    gly_count = cmd.count_atoms("gly")

    with tempfile.NamedTemporaryFile(suffix='.bcif', delete=False) as f:
        bcif_file = f.name

    try:
        cmd.save(bcif_file, "all")
        assert os.path.getsize(bcif_file) > 0

        cmd.delete("all")
        cmd.load(bcif_file)

        names = cmd.get_object_list()
        assert len(names) == 2, f"Expected 2 objects, got {len(names)}: {names}"
        assert cmd.count_atoms(names[0]) == ala_count
        assert cmd.count_atoms(names[1]) == gly_count
    finally:
        if os.path.exists(bcif_file):
            os.unlink(bcif_file)

import numpy
import pytest
from pymol import cmd
from pymol import test_utils


def test_self_alignment():
    """Self-alignment should give TM-score ~1.0 and RMSD ~0.0"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.create("m2", "m1")
    r = cmd.usalign("m2", "m1", transform=0)
    assert isinstance(r, dict)
    assert r["tm_score_target"] == pytest.approx(1.0, abs=0.01)
    assert r["tm_score_mobile"] == pytest.approx(1.0, abs=0.01)
    assert r["RMSD"] == pytest.approx(0.0, abs=0.1)


def test_cross_alignment():
    """Cross-alignment of two similar fragments should give reasonable scores"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.load(test_utils.datafile("1t46-frag.pdb"), "m2")
    r = cmd.usalign("m1", "m2", transform=0)
    assert isinstance(r, dict)
    assert r["tm_score_target"] > 0.3
    assert r["alignment_length"] > 10
    assert r["RMSD"] > 0.0


def test_alignment_object():
    """object= should create a named alignment object"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.create("m2", "m1")
    r = cmd.usalign("m2", "m1", object="aln", transform=0)
    assert isinstance(r, dict)
    assert "aln" in cmd.get_names()


def test_no_transform():
    """transform=0 should not move the mobile object"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.create("m2", "m1")
    coords_before = numpy.array(cmd.get_coords("m2", 1))
    cmd.usalign("m2", "m1", transform=0)
    coords_after = numpy.array(cmd.get_coords("m2", 1))
    assert numpy.allclose(coords_before, coords_after, atol=1e-6)


def test_return_dict():
    """Return value should contain all expected keys"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.create("m2", "m1")
    r = cmd.usalign("m2", "m1", transform=0)
    assert isinstance(r, dict)
    for key in ["tm_score_target", "tm_score_mobile", "RMSD",
                "alignment_length", "seq_identity"]:
        assert key in r


def test_fast_mode():
    """Fast mode should still produce valid results"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.create("m2", "m1")
    r = cmd.usalign("m2", "m1", fast=1, transform=0)
    assert isinstance(r, dict)
    assert r["tm_score_target"] > 0.5


def test_dissimilar_structures():
    """Dissimilar structures should give low TM-scores"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "m1")
    cmd.load(test_utils.datafile("1rx1.pdb"), "m2")
    r = cmd.usalign("m1", "m2", transform=0)
    assert isinstance(r, dict)
    assert r["tm_score_target"] < 0.3
    assert r["tm_score_mobile"] < 0.5
    assert r["alignment_length"] > 0


def test_protein_vs_nucleic_acid():
    """Protein vs nucleic acid should run without error and give low TM-scores"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "protein")
    cmd.load(test_utils.datafile("1bna.cif"), "dna")
    r = cmd.usalign("protein", "dna", transform=0)
    assert isinstance(r, dict)
    assert r["tm_score_target"] < 0.3
    assert r["alignment_length"] > 0


def test_alignto_usalign():
    """alignto should work with method=usalign"""
    cmd.load(test_utils.datafile("1oky-frag.pdb"), "ref")
    cmd.load(test_utils.datafile("1t46-frag.pdb"), "obj1")
    cmd.load(test_utils.datafile("1rx1.pdb"), "obj2")
    cmd.alignto("ref", method="usalign")
    assert cmd.count_atoms("obj1") > 0
    assert cmd.count_atoms("obj2") > 0


def test_too_few_guide_atoms():
    """Selections with fewer than 3 guide atoms should raise RuntimeError"""
    cmd.fragment("gly")
    cmd.create("m2", "gly")
    with pytest.raises(RuntimeError, match="fewer than 3 guide atoms"):
        cmd.usalign("gly", "m2")

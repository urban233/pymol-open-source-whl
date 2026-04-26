from pymol import cmd
from pymol import test_utils


def test_select_operators():
    cmd.reinitialize()
    cmd.pseudoatom(pos=[1, 2, 3], b=5)

    assert cmd.count_atoms("b > 4") == 1
    assert cmd.count_atoms("b < 6") == 1
    assert cmd.count_atoms("b = 5") == 1
    assert cmd.count_atoms("b >= 5") == 1
    assert cmd.count_atoms("b <= 5") == 1

    assert cmd.count_atoms("b > 4 & x == 1") == 1
    assert cmd.count_atoms("b > 5 & x == 1") == 0
    assert cmd.count_atoms("b < 6 & y <= 3") == 1
    assert cmd.count_atoms("b = 5 & z >= 2") == 1


def test_pna_classified_as_nucleic():
    """PNA residues (CPN/TPN/APN/GPN) should be classified as polymer.nucleic"""
    cmd.load(test_utils.datafile("1pup.cif"))
    pna_sele = "resn CPN+TPN+APN+GPN"
    n_pna = cmd.count_atoms(pna_sele)
    assert n_pna > 0, "no PNA residues found"
    n_nuc = cmd.count_atoms(pna_sele + " & polymer.nucleic")
    assert n_nuc == n_pna, \
        f"only {n_nuc}/{n_pna} PNA atoms classified as polymer.nucleic"


def test_pna_guide_atoms_are_polymer():
    """PNA guide atoms should be flagged as polymer"""
    cmd.load(test_utils.datafile("1pup.cif"))
    pna_sele = "resn CPN+TPN+APN+GPN"
    n_guide = cmd.count_atoms(pna_sele + " & guide & polymer")
    assert n_guide > 0, "PNA has no polymer guide atoms"


def test_dna_rna_unchanged():
    """Existing DNA/RNA classification should be unaffected"""
    cmd.load(test_utils.datafile("1ehz-5.pdb"))
    assert cmd.count_atoms("polymer.nucleic") == 112
    assert cmd.count_atoms("guide") > 0

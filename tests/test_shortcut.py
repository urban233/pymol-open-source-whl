import pytest

from pymol.shortcut import Shortcut


pytestmark = pytest.mark.small


@pytest.fixture
def sc() -> Shortcut:
    return Shortcut(["foo", "bar", "baz", "com", "com_bla", "com_xxx"])


@pytest.mark.parametrize(
    "keyword, expected_result",
    [
        ("a", False),
        ("w", True),
        ("war", True),
    ],
)
def test_contains(keyword: str, expected_result: bool):
    shortcut = Shortcut(["warren", "wasteland", "electric", "well"])
    assert (keyword in shortcut) is expected_result


def test_interpret():
    shortcut = Shortcut(["warren", "wasteland", "electric", "well"])
    list_result = shortcut.interpret("w")
    assert list_result is not None
    assert not isinstance(list_result, int)
    assert sorted(list_result) == ["warren", "wasteland", "well"]

    string_result = shortcut.interpret("e")
    assert list_result is not None
    assert string_result == "electric"


def test_all_keywords(sc: Shortcut):
    assert ["foo", "bar", "baz", "com", "com_bla", "com_xxx"] == sc.interpret("")


@pytest.mark.parametrize(
    "prefixes, expected_result",
    [
        (["f", "fo", "foo"], "foo"),
        (["b", "ba"], ["bar", "baz"]),
        (["bar"], "bar"),
        (["c", "co"], ["com", "com_bla", "com_xxx"]),
        (["com"], "com"),
    ],
)
def test_full_prefix_hits(
    sc: Shortcut, prefixes: list[str], expected_result: str | list[str]
):
    for prefix in prefixes:
        result = sc.interpret(prefix)
        result = sorted(result) if isinstance(result, list) else result
        assert expected_result == result


def test_append(sc: Shortcut):
    sc.append("foo_new")

    assert sorted(sc.interpret("f")) == ["foo", "foo_new"]
    assert sc.interpret("foo") == "foo"
    assert sc.interpret("foo_") == "foo_new"
    assert "" not in sc


def test_abbreviations(sc: Shortcut):
    sc.append("foo_new")

    assert sc.interpret("f_") == "foo_new"
    assert sc.interpret("f_new") == "foo_new"
    assert sc.interpret("fo_") == "foo_new"
    assert sc.interpret("c_x") == "com_xxx"
    assert sc.interpret("c_xxx") == "com_xxx"
    assert sc.interpret("co_x") == "com_xxx"


def test_missing_key(sc: Shortcut):
    assert sc.interpret("missing_key") is None


def test_auto_error(sc: Shortcut):
    assert sc.auto_err("") is None
    assert sc.auto_err("missing_key") is None

    result = sc.auto_err("co")
    assert isinstance(result, list)
    assert ["com", "com_bla", "com_xxx"] == sorted(result)
    assert sc.auto_err("com") == "com"


def test_interpret_mode_true(sc: Shortcut):
    assert sc.interpret("f", True) == "foo"

    result = sc.interpret("com", True)
    assert isinstance(result, list)
    assert ["com", "com_bla", "com_xxx"] == sorted(result)

    sc.append("foo_new")
    result = sc.interpret("foo", True)
    assert isinstance(result, list)
    assert ["foo", "foo_new"] == sorted(result)


def test_rebuild(sc: Shortcut):
    commands = ["com", "com_bla", "com_xxx"]
    sc.rebuild(commands)

    assert sc.interpret("f") is None
    assert sc.interpret("foo") is None

    result = sc.interpret("c")
    assert isinstance(result, list)
    assert commands == sorted(result)

    result = sc.interpret("com", True)
    assert isinstance(result, list)
    assert commands == sorted(result)

    assert sc.interpret("com") == "com"
    assert sc.interpret("c_x") == "com_xxx"

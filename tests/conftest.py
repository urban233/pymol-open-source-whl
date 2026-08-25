"""Shared pytest configuration for the test suite."""

import pytest


_SIZE_MARKERS = ("small", "medium", "large")


def pytest_collection_modifyitems(items: list[pytest.Item]) -> None:
    """Require every test item to declare exactly one size marker.

    Args:
        items: Collected pytest items to validate.
    """
    for item in items:
        size_markers = [
            marker
            for name in _SIZE_MARKERS
            for marker in item.iter_markers(name=name)
        ]
        if len(size_markers) != 1:
            names = [marker.name for marker in size_markers]
            raise pytest.CollectError(
                f"{item.nodeid} must have exactly one size marker "
                f"(small, medium, or large); found {names or 'none'}"
            )

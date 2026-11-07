#!/usr/bin/env python3
"""Test the installed wheel against the minimum and latest supported NumPy 2."""

from __future__ import annotations

import importlib.metadata
import os
import platform
import subprocess
import sys
import sysconfig
import xml.etree.ElementTree as ET
from pathlib import Path

PROJECT = Path(__file__).resolve().parents[1]
SMOKE_CASES = {
    "test_numpy_coordinates[float32]",
    "test_numpy_coordinates[float64]",
    "test_numpy_volume",
}


def verify_smoke_results(report: Path) -> None:
    """Fail closed if mandatory native tests disappeared or were skipped."""
    cases = {
        case.attrib["name"]: case
        for case in ET.parse(report).iter("testcase")
        if case.attrib.get("classname", "").endswith("test_wheel_smoke")
    }
    if not SMOKE_CASES.issubset(cases):
        raise RuntimeError(
            f"Missing mandatory wheel tests: {SMOKE_CASES - cases.keys()}"
        )
    for name in SMOKE_CASES:
        if any(
            cases[name].find(tag) is not None for tag in ("skipped", "error", "failure")
        ):
            raise RuntimeError(f"Mandatory wheel test did not pass: {name}")


def main() -> None:
    # NumPy 2.2.0 supports Python 3.10-3.13; 2.3.3 adds Python 3.14 wheels.
    minimum = "2.3.3" if sys.version_info >= (3, 14) else "2.2.0"
    reports = Path(
        os.environ.get(
            "PYMOL_WHEEL_TEST_REPORTS", PROJECT / "build" / "wheel-test-results"
        )
    )
    reports.mkdir(parents=True, exist_ok=True)
    wheel_platform = f"{sys.implementation.cache_tag}-{sysconfig.get_platform()}"
    for profile, requirement in (
        ("minimum", f"numpy=={minimum}"),
        ("latest", "numpy>=2.2,<3"),
    ):
        subprocess.run(
            [
                sys.executable,
                "-m",
                "pip",
                "install",
                "--only-binary=:all:",
                "--upgrade",
                requirement,
            ],
            check=True,
        )
        numpy_version = importlib.metadata.version("numpy")
        print(
            f"Wheel test: {profile}; Python {platform.python_version()}; "
            f"{platform.platform()}; NumPy {numpy_version}; "
            f"PyMOL {importlib.metadata.version('pymol-open-source-whl')}",
            flush=True,
        )
        report = reports / f"{wheel_platform}-{profile}-numpy-{numpy_version}.xml"
        subprocess.run(
            [sys.executable, "-m", "pip", "check"],
            check=True,
        )
        subprocess.run(
            [
                sys.executable,
                "-m",
                "pytest",
                str(PROJECT / "tests"),
                "-ra",
                f"--junitxml={report}",
            ],
            check=True,
        )
        verify_smoke_results(report)


if __name__ == "__main__":
    main()

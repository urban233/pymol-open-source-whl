#!/usr/bin/env python3
"""Verify wheel metadata and coverage for a PyPI release."""

from __future__ import annotations

import argparse
import sys
import tomllib
import zipfile
from email import policy
from email.parser import BytesParser
from pathlib import Path


PROJECT_NAME = "pymol-open-source-whl"
PYTHON_TAGS = {f"cp3{minor}" for minor in range(10, 15)}
PLATFORM_FAMILIES = {
    "windows-x64",
    "macos-intel",
    "macos-arm64",
    "linux-x64",
}


def read_project_version(project_file: Path) -> str:
    with project_file.open("rb") as stream:
        project = tomllib.load(stream)
    try:
        name = project["project"]["name"]
        version = project["project"]["version"]
    except KeyError as error:
        raise ValueError(f"Missing project metadata in {project_file}: {error}") from error
    if name != PROJECT_NAME:
        raise ValueError(f"Expected project name {PROJECT_NAME!r}, found {name!r}")
    return version


def platform_family(platform_tag: str) -> str | None:
    if platform_tag == "win32":
        return "windows-x86"
    if platform_tag == "win_amd64":
        return "windows-x64"
    if platform_tag.endswith("_x86_64"):
        if platform_tag.startswith("macosx"):
            return "macos-intel"
        if platform_tag.startswith("manylinux"):
            return "linux-x64"
    if platform_tag.endswith("_arm64") and platform_tag.startswith("macosx"):
        return "macos-arm64"
    return None


def wheel_metadata(wheel: Path) -> tuple[str, str]:
    with zipfile.ZipFile(wheel) as archive:
        metadata_files = [
            name for name in archive.namelist() if name.endswith(".dist-info/METADATA")
        ]
        if len(metadata_files) != 1:
            raise ValueError(f"{wheel.name}: expected one dist-info/METADATA file")
        metadata = BytesParser(policy=policy.default).parsebytes(
            archive.read(metadata_files[0])
        )
    name = metadata.get("Name")
    version = metadata.get("Version")
    if not name or not version:
        raise ValueError(f"{wheel.name}: metadata must contain Name and Version")
    return name, version


def verify_wheel(wheel: Path, version: str) -> tuple[str, str]:
    parts = wheel.name.removesuffix(".whl").split("-")
    if len(parts) != 5:
        raise ValueError(f"{wheel.name}: invalid wheel filename")
    name, wheel_version, python_tag, _abi_tag, platform_tag = parts
    if name != PROJECT_NAME.replace("-", "_"):
        raise ValueError(f"{wheel.name}: unexpected distribution name")
    if wheel_version != version:
        raise ValueError(f"{wheel.name}: expected version {version}, found {wheel_version}")
    if python_tag not in PYTHON_TAGS:
        raise ValueError(f"{wheel.name}: unexpected Python tag {python_tag}")
    family = platform_family(platform_tag)
    if family is None:
        raise ValueError(f"{wheel.name}: unsupported platform tag {platform_tag}")

    metadata_name, metadata_version = wheel_metadata(wheel)
    if metadata_name != PROJECT_NAME:
        raise ValueError(f"{wheel.name}: embedded project name is {metadata_name!r}")
    if metadata_version != version:
        raise ValueError(
            f"{wheel.name}: embedded version is {metadata_version!r}, expected {version!r}"
        )
    return python_tag, family


def verify_release(dist: Path, project_file: Path, tag: str | None) -> None:
    version = read_project_version(project_file)
    if tag is not None and tag != f"v{version}":
        raise ValueError(f"Release tag {tag!r} does not match expected tag v{version}")

    wheels = sorted(dist.glob("*.whl"))
    if not wheels:
        raise ValueError(f"No wheel files found in {dist}")
    observed = {verify_wheel(wheel, version) for wheel in wheels}
    expected = {(python_tag, family) for python_tag in PYTHON_TAGS for family in PLATFORM_FAMILIES}
    missing = sorted(expected - observed)
    unexpected = sorted(observed - expected)
    if missing or unexpected or len(wheels) != len(expected):
        details = []
        if missing:
            details.append(f"missing={missing}")
        if unexpected:
            details.append(f"unexpected={unexpected}")
        if len(wheels) != len(expected):
            details.append(f"wheel_count={len(wheels)}, expected={len(expected)}")
        raise ValueError("Wheel coverage mismatch: " + "; ".join(details))

    print(f"Verified {len(wheels)} {PROJECT_NAME} {version} wheels")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dist", type=Path, default=Path("dist"))
    parser.add_argument("--project-file", type=Path, default=Path("pyproject.toml"))
    parser.add_argument("--tag", help="Release tag to validate, for example v3.2.0.1")
    args = parser.parse_args()
    try:
        verify_release(args.dist, args.project_file, args.tag)
    except (OSError, ValueError, zipfile.BadZipFile) as error:
        print(f"Release verification failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

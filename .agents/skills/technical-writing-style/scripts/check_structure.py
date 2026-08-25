#!/usr/bin/env python3
"""Supplemental, dependency-free structural checks for a Markdown document set.

Catches what a re-read is prone to miss across several related documents: a
broken cross-file link or anchor, a heading with no content before its first
subheading, a table column that repeats nearly the same value in every row,
a table wide enough that its cells are carrying prose instead of comparable
values, and a paragraph that is a near-duplicate of one in a different file.
Every check here is mechanical -- it does not replace the judgment calls in
`references/writing-style.md` (whether a split is justified, whether a
sentence should be a list, whether jargon needs defining), only the parts of
that reference that can be verified by pattern-matching instead of reading.
"""

from __future__ import annotations

import argparse
import dataclasses
import json
import pathlib
import re
import sys

_HEADING_PATTERN = re.compile(r"^(#{1,6})\s+(.*)$")
_LINK_PATTERN = re.compile(r"\[([^\]]+)\]\(([^)]+)\)")
_TABLE_ROW_PATTERN = re.compile(r"^\s*\|(.+)\|\s*$")
_TABLE_SEPARATOR_PATTERN = re.compile(r"^\s*\|?[\s:|-]+\|?\s*$")
_METADATA_FIELD_PATTERN = re.compile(r"^\*\*[^*]+:\*\*")
_LIST_ITEM_PATTERN = re.compile(r"^([-*+]|\d+\.)\s")
_STOPWORDS = frozenset(
    {
        "a", "an", "and", "are", "as", "at", "be", "but", "by", "for", "from",
        "has", "have", "if", "in", "into", "is", "it", "its", "no", "not",
        "of", "on", "only", "or", "our", "that", "the", "their", "then",
        "this", "to", "was", "were", "when", "which", "with",
    }
)
_MIN_DUPLICATE_WORDS = 8
_DUPLICATE_JACCARD_THRESHOLD = 0.6
_REDUNDANT_COLUMN_RATIO = 0.8
_REDUNDANT_COLUMN_MIN_ROWS = 3
_WIDE_TABLE_COLUMNS = 5
_DENSE_CELL_CHARS = 150


@dataclasses.dataclass(frozen=True)
class Finding:
    """One structural-audit finding."""

    level: str
    rule: str
    file: str
    line: int
    column: int
    message: str


def _slugify(heading_text: str) -> str:
    """Approximate GitHub's heading-to-anchor slug rules."""
    text = heading_text.lower()
    text = re.sub(r"[`*]", "", text)
    text = re.sub(r"[^a-z0-9\- ]", "", text)
    text = text.strip().replace(" ", "-")
    return re.sub(r"-+", "-", text)


def _read_lines(file_path: pathlib.Path) -> list[str]:
    return file_path.read_text(encoding="utf-8").splitlines()


def _headings(lines: list[str]) -> list[tuple[int, int, str]]:
    """Return (line_number, level, slug) for every heading."""
    found = []
    in_fence = False
    for index, line in enumerate(lines):
        if line.strip().startswith("```"):
            in_fence = not in_fence
            continue
        if in_fence:
            continue
        match = _HEADING_PATTERN.match(line)
        if match:
            found.append((index + 1, len(match.group(1)), _slugify(match.group(2))))
    return found


def _check_links(
    file_path: pathlib.Path,
    lines: list[str],
    all_headings: dict[str, set[str]],
) -> list[Finding]:
    """Flag a link target that does not exist or an anchor with no matching heading."""
    findings: list[Finding] = []
    for line_number, line in enumerate(lines, start=1):
        for _label, target in _LINK_PATTERN.findall(line):
            if target.startswith(("http://", "https://", "mailto:")):
                continue
            path_part, _, anchor = target.partition("#")
            resolved = (file_path.parent / path_part).resolve() if path_part else file_path
            if path_part and not resolved.is_file():
                findings.append(
                    Finding("violation", "broken-link", file_path.name, line_number, 1,
                             f"Link target does not exist: {target!r}.")
                )
                continue
            if anchor:
                key = resolved.name if path_part else file_path.name
                if anchor not in all_headings.get(key, set()):
                    findings.append(
                        Finding("violation", "broken-anchor", file_path.name, line_number, 1,
                                 f"No heading in {key!r} produces anchor #{anchor}.")
                    )
    return findings


def _check_stacked_headings(file_path: pathlib.Path, lines: list[str]) -> list[Finding]:
    """Flag a heading with no content before the next heading."""
    findings: list[Finding] = []
    in_fence = False
    for index in range(len(lines) - 2):
        if lines[index].strip().startswith("```"):
            in_fence = not in_fence
        if in_fence:
            continue
        if (
            _HEADING_PATTERN.match(lines[index])
            and lines[index + 1].strip() == ""
            and _HEADING_PATTERN.match(lines[index + 2])
        ):
            findings.append(
                Finding("violation", "stacked-heading", file_path.name, index + 1, 1,
                         f"{lines[index].strip()!r} has no content before "
                         f"{lines[index + 2].strip()!r}.")
            )
    return findings


def _parse_tables(lines: list[str]) -> list[tuple[int, list[list[str]]]]:
    """Return (header_line_number, rows_including_header) for every table."""
    tables: list[tuple[int, list[list[str]]]] = []
    index = 0
    while index < len(lines) - 1:
        header_match = _TABLE_ROW_PATTERN.match(lines[index])
        if header_match and _TABLE_SEPARATOR_PATTERN.match(lines[index + 1]):
            start_line = index + 1
            rows = [[cell.strip() for cell in header_match.group(1).split("|")]]
            cursor = index + 2
            while cursor < len(lines):
                row_match = _TABLE_ROW_PATTERN.match(lines[cursor])
                if not row_match:
                    break
                rows.append([cell.strip() for cell in row_match.group(1).split("|")])
                cursor += 1
            tables.append((start_line, rows))
            index = cursor
        else:
            index += 1
    return tables


def _check_tables(file_path: pathlib.Path, lines: list[str]) -> list[Finding]:
    """Flag a redundant column, an over-wide table, or a cell carrying prose."""
    findings: list[Finding] = []
    for start_line, rows in _parse_tables(lines):
        header, data_rows = rows[0], rows[1:]
        if len(header) > _WIDE_TABLE_COLUMNS:
            findings.append(
                Finding("review", "wide-table", file_path.name, start_line, 1,
                         f"Table {header!r} has {len(header)} columns; move dense "
                         "columns into a bulleted block under each row instead.")
            )
        if len(data_rows) >= _REDUNDANT_COLUMN_MIN_ROWS:
            for col_index, col_name in enumerate(header):
                values = [row[col_index] for row in data_rows if col_index < len(row)]
                if not values:
                    continue
                most_common = max(values, key=values.count)
                if values.count(most_common) / len(values) >= _REDUNDANT_COLUMN_RATIO:
                    findings.append(
                        Finding("review", "redundant-column", file_path.name, start_line, 1,
                                 f"Column {col_name!r} is {most_common!r} in "
                                 f"{values.count(most_common)}/{len(values)} rows; state it "
                                 "once in prose and drop the column.")
                    )
        for row in data_rows:
            for cell in row:
                if len(cell) > _DENSE_CELL_CHARS or re.search(r"\.\s+[A-Z]", cell):
                    findings.append(
                        Finding("review", "dense-cell", file_path.name, start_line, 1,
                                 f"Cell carries prose, not a comparable value: "
                                 f"{cell[:80]!r}...")
                    )
    return findings


def _paragraphs(file_path: pathlib.Path, lines: list[str]) -> list[tuple[int, str]]:
    """Return (line_number, text) for prose paragraphs.

    Excludes code, tables, headings, document-metadata fields
    (``**Status:** Draft``), and list/checklist items -- none of those are
    prose a reader would recognize as "the same claim stated twice," and
    metadata fields and template checklists are *supposed* to look near-
    identical across sibling documents that share a template.
    """
    found: list[tuple[int, str]] = []
    buffer: list[str] = []
    start = 0
    in_fence = False

    def flush() -> None:
        if buffer:
            text = " ".join(buffer).strip()
            if len(text.split()) >= _MIN_DUPLICATE_WORDS:
                found.append((start, text))
        buffer.clear()

    for index, line in enumerate(lines):
        stripped = line.strip()
        if stripped.startswith("```"):
            in_fence = not in_fence
            flush()
            continue
        if (
            in_fence
            or _HEADING_PATTERN.match(line)
            or stripped.startswith("|")
            or _METADATA_FIELD_PATTERN.match(stripped)
            or _LIST_ITEM_PATTERN.match(stripped)
            or not stripped
        ):
            flush()
            continue
        if not buffer:
            start = index + 1
        buffer.append(stripped)
    flush()
    return found


def _word_set(text: str) -> set[str]:
    words = re.findall(r"[a-z0-9]+", text.lower())
    return {word for word in words if word not in _STOPWORDS}


def _check_cross_file_duplicates(
    all_paragraphs: dict[str, list[tuple[int, str]]],
) -> list[Finding]:
    """Flag a paragraph that closely matches one in a different file."""
    findings: list[Finding] = []
    files = sorted(all_paragraphs)
    for i, file_a in enumerate(files):
        for file_b in files[i + 1 :]:
            for line_a, text_a in all_paragraphs[file_a]:
                words_a = _word_set(text_a)
                if not words_a:
                    continue
                for line_b, text_b in all_paragraphs[file_b]:
                    words_b = _word_set(text_b)
                    if not words_b:
                        continue
                    overlap = len(words_a & words_b) / len(words_a | words_b)
                    if overlap >= _DUPLICATE_JACCARD_THRESHOLD:
                        findings.append(
                            Finding(
                                "review", "cross-file-duplicate", file_a, line_a, 1,
                                f"{overlap:.0%} word overlap with {file_b}:{line_b} -- "
                                "state this fact once and link to it instead of "
                                f"restating it. {file_a}: {text_a[:70]!r}... / "
                                f"{file_b}: {text_b[:70]!r}...",
                            )
                        )
    return findings


def _run(files: list[pathlib.Path]) -> list[Finding]:
    all_lines = {file_path.name: _read_lines(file_path) for file_path in files}
    all_headings = {
        name: {slug for _line, _level, slug in _headings(lines)}
        for name, lines in all_lines.items()
    }
    findings: list[Finding] = []
    for file_path in files:
        lines = all_lines[file_path.name]
        findings.extend(_check_links(file_path, lines, all_headings))
        findings.extend(_check_stacked_headings(file_path, lines))
        findings.extend(_check_tables(file_path, lines))
    if len(files) > 1:
        paragraphs = {
            file_path.name: _paragraphs(file_path, all_lines[file_path.name])
            for file_path in files
        }
        findings.extend(_check_cross_file_duplicates(paragraphs))
    return findings


def _target_files(args: argparse.Namespace) -> list[pathlib.Path]:
    if args.files:
        return [pathlib.Path(path).resolve() for path in args.files]
    return sorted(args.root.resolve().glob("*.md"))


def main() -> int:
    """Run the structural audit and emit JSON-lines findings."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("files", nargs="*", help="Explicit Markdown files to check together.")
    parser.add_argument(
        "--root", type=pathlib.Path, default=pathlib.Path.cwd(),
        help="When no files are given, check every *.md file directly in this directory.",
    )
    args = parser.parse_args()
    files = _target_files(args)
    if not files:
        print("No Markdown files found.", file=sys.stderr)
        return 0
    findings = _run(files)
    for finding in findings:
        print(json.dumps(dataclasses.asdict(finding), sort_keys=True))
    violations = sum(finding.level == "violation" for finding in findings)
    reviews = sum(finding.level == "review" for finding in findings)
    print(
        f"Checked {len(files)} Markdown files: {violations} violations, "
        f"{reviews} review items.",
        file=sys.stderr,
    )
    return 1 if violations else 0


if __name__ == "__main__":
    raise SystemExit(main())

#!/usr/bin/env python3
"""Parse lcov .info and evaluate green-branch coverage gates."""
from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass
from datetime import date
from pathlib import Path


@dataclass
class FileCoverage:
    path: str
    lines_hit: int
    lines_total: int
    branches_hit: int
    branches_total: int
    miss_lines: list[int]

    @property
    def line_pct(self) -> float:
        return 100.0 * self.lines_hit / self.lines_total if self.lines_total else 100.0

    @property
    def branch_pct(self) -> float:
        return (
            100.0 * self.branches_hit / self.branches_total
            if self.branches_total
            else 100.0
        )


def _norm_path(sf: str) -> str:
    p = sf.replace("\\", "/")
    for marker in (
        "/FeedbackAnalyzer_09/",
        "/FeedbackAnalyzer_09\\",
    ):
        idx = p.find("FeedbackAnalyzer_09/")
        if idx >= 0:
            return p[idx + len("FeedbackAnalyzer_09/") :]
    if "tests/support/" in p:
        return p[p.index("tests/support/") :]
    if "src/cpp/" in p:
        return p[p.index("src/cpp/") :]
    return p


def parse_lcov(info_path: Path) -> dict[str, FileCoverage]:
    text = info_path.read_text(encoding="utf-8", errors="replace")
    files: dict[str, FileCoverage] = {}
    for block in text.split("end_of_record"):
        block = block.strip()
        if not block:
            continue
        sf_match = re.search(r"^SF:(.+)$", block, re.M)
        if not sf_match:
            continue
        rel = _norm_path(sf_match.group(1))
        das = re.findall(r"^DA:(\d+),(\d+)", block, re.M)
        if not das:
            continue
        lines_hit = sum(1 for _, c in das if int(c) > 0)
        lines_total = len(das)
        miss = sorted(int(ln) for ln, c in das if int(c) == 0)
        brs = re.findall(r"^BRDA:(\d+),(\d+),(\d+),(\d+|-)", block, re.M)
        br_hit = sum(1 for *_, taken in brs if taken not in ("0", "-"))
        br_total = sum(1 for *_, taken in brs if taken != "-")
        files[rel] = FileCoverage(
            path=rel,
            lines_hit=lines_hit,
            lines_total=lines_total,
            branches_hit=br_hit,
            branches_total=br_total,
            miss_lines=miss,
        )
    return files


def aggregate(files: dict[str, FileCoverage], paths: list[str]) -> FileCoverage:
    hit = total = br_hit = br_total = 0
    miss: list[int] = []
    found: list[str] = []
    for rel in paths:
        cov = files.get(rel)
        if not cov or cov.lines_total == 0:
            continue
        found.append(rel)
        hit += cov.lines_hit
        total += cov.lines_total
        br_hit += cov.branches_hit
        br_total += cov.branches_total
        miss.extend(cov.miss_lines)
    label = "+".join(found) if found else "(none)"
    return FileCoverage(
        path=label,
        lines_hit=hit,
        lines_total=total,
        branches_hit=br_hit,
        branches_total=br_total,
        miss_lines=sorted(set(miss)),
    )


def boundary_metrics(
    info_path: Path, boundary_lines: dict[str, list[int]]
) -> tuple[int, int, int, int, list[str]]:
    text = info_path.read_text(encoding="utf-8", errors="replace")
    ln_hit = ln_total = br_hit = br_total = 0
    miss_notes: list[str] = []
    for block in text.split("end_of_record"):
        if not block.strip():
            continue
        sf_match = re.search(r"^SF:(.+)$", block, re.M)
        if not sf_match:
            continue
        rel = _norm_path(sf_match.group(1))
        watch = boundary_lines.get(rel)
        if not watch:
            continue
        das = {int(ln): int(c) for ln, c in re.findall(r"^DA:(\d+),(\d+)", block, re.M)}
        brs = re.findall(r"^BRDA:(\d+),(\d+),(\d+),(\d+|-)", block, re.M)
        for ln in watch:
            if ln in das:
                ln_total += 1
                if das[ln] > 0:
                    ln_hit += 1
                else:
                    miss_notes.append(f"{rel}:{ln}")
        for ln in watch:
            for br_ln, _, _, taken in brs:
                if int(br_ln) != ln or taken == "-":
                    continue
                br_total += 1
                if taken != "0":
                    br_hit += 1
    return ln_hit, ln_total, br_hit, br_total, miss_notes


def main() -> int:
    if len(sys.argv) < 4:
        print(
            "Usage: parse_coverage_gate.py <coverage.info> <manifest.json> "
            "<report.md> [--domain=90] [--boundary=85] [--overall=90]",
            file=sys.stderr,
        )
        return 2

    info_path = Path(sys.argv[1])
    manifest_path = Path(sys.argv[2])
    report_path = Path(sys.argv[3])
    thresholds = {"domain": 90.0, "boundary": 85.0, "overall": 90.0}
    for arg in sys.argv[4:]:
        if arg.startswith("--") and "=" in arg:
            key, val = arg[2:].split("=", 1)
            if key in thresholds:
                thresholds[key] = float(val)

    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    files = parse_lcov(info_path)

    domain = aggregate(files, manifest["domain_files"])
    overall_paths = manifest.get("overall_files", manifest["domain_files"])
    overall = aggregate(files, overall_paths)
    bl_hit, bl_tot, bb_hit, bb_tot, b_miss = boundary_metrics(
        info_path, manifest["boundary_lines"]
    )
    boundary_line_pct = 100.0 * bl_hit / bl_tot if bl_tot else 100.0
    boundary_branch_pct = 100.0 * bb_hit / bb_tot if bb_tot else 100.0

    checks = [
        ("Domain", domain.line_pct, thresholds["domain"]),
        ("Boundary (branches @ boundary lines)", boundary_branch_pct, thresholds["boundary"]),
        ("Overall", overall.line_pct, thresholds["overall"]),
    ]
    passed = all(pct >= thr for _, pct, thr in checks)

    lines = [
        "# Coverage Report (green)",
        "",
        f"| Generated | {date.today().isoformat()} |",
        f"| Info file | `{info_path.as_posix()}` |",
        "",
        "## Gate summary",
        "",
        "| Metric | Threshold | Measured | Status |",
        "|--------|-----------|----------|--------|",
    ]
    for name, pct, thr in checks:
        status = "PASS" if pct >= thr else "**FAIL**"
        lines.append(f"| {name} | >= {thr:.0f}% | {pct:.1f}% | {status} |")
    lines.append(f"| Boundary lines (aux) | — | {boundary_line_pct:.1f}% ({bl_hit}/{bl_tot}) | — |")
    lines.append("")
    lines.append("## Per-file (Domain)")
    lines.append("")
    lines.append("| File | Line % | Hit/Total | Miss lines | Branch % |")
    lines.append("|------|--------|-----------|------------|----------|")
    for rel in manifest["domain_files"]:
        cov = files.get(rel)
        if not cov or cov.lines_total == 0:
            lines.append(f"| `{rel}` | — | 0/0 | (no data) | — |")
            continue
        miss = ", ".join(str(x) for x in cov.miss_lines) or "—"
        lines.append(
            f"| `{rel}` | {cov.line_pct:.1f}% | {cov.lines_hit}/{cov.lines_total} | {miss} | {cov.branch_pct:.1f}% |"
        )
    if b_miss:
        lines.extend(["", "## Boundary misses", ""])
        for note in b_miss:
            lines.append(f"- `{note}`")
    lines.extend(
        [
            "",
            "## Commands",
            "",
            "```powershell",
            ".\\scripts\\run_coverage_gate.ps1 -BuildDir build-cov",
            "```",
            "",
            "```bash",
            "./scripts/run_coverage_gate.sh build-cov",
            "```",
            "",
        ]
    )
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text("\n".join(lines), encoding="utf-8")

    print(f"Domain line coverage: {domain.line_pct:.1f}% ({domain.lines_hit}/{domain.lines_total})")
    print(
        f"Boundary branch coverage: {boundary_branch_pct:.1f}% ({bb_hit}/{bb_tot})"
    )
    print(f"Overall line coverage: {overall.line_pct:.1f}% ({overall.lines_hit}/{overall.lines_total})")
    print(f"Report: {report_path}")
    return 0 if passed else 1


if __name__ == "__main__":
    raise SystemExit(main())

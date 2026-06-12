#!/usr/bin/env bash
# Feedback Analyzer — green branch coverage gate (Domain / Boundary / Overall)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:-build-cov}"
DOMAIN_THRESHOLD="${DOMAIN_THRESHOLD:-90}"
BOUNDARY_THRESHOLD="${BOUNDARY_THRESHOLD:-85}"
OVERALL_THRESHOLD="${OVERALL_THRESHOLD:-90}"

cd "$ROOT"

command -v gcov >/dev/null || { echo "gcov not found"; exit 1; }
command -v lcov >/dev/null || { echo "lcov not found"; exit 1; }
command -v cmake >/dev/null || { echo "cmake not found"; exit 1; }
command -v python3 >/dev/null || command -v python >/dev/null || { echo "python not found"; exit 1; }
PY="$(command -v python3 2>/dev/null || command -v python)"

if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
  cmake -S . -B "$BUILD_DIR" -G Ninja -DFA_ENABLE_COVERAGE=ON
fi

cmake --build "$BUILD_DIR" --target feedback_analyzer_tests
TEST_EXE="$BUILD_DIR/feedback_analyzer_tests"
[[ -x "$TEST_EXE" ]] || TEST_EXE="$BUILD_DIR/feedback_analyzer_tests.exe"

pushd "$BUILD_DIR" >/dev/null
lcov --directory . --zerocounters
"$TEST_EXE"
lcov --directory . --capture --output-file coverage.info --rc lcov_branch_coverage=1
popd >/dev/null

"$PY" "$ROOT/scripts/parse_coverage_gate.py" \
  "$BUILD_DIR/coverage.info" \
  "$ROOT/scripts/coverage_boundary.json" \
  "$ROOT/docs/coverage_report.md" \
  "--domain=$DOMAIN_THRESHOLD" \
  "--boundary=$BOUNDARY_THRESHOLD" \
  "--overall=$OVERALL_THRESHOLD"

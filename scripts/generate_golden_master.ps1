# Regenerate tests/golden/*.approved.txt from Domain support.
# Prerequisites: FA-TC + coverage gate Green (see docs/golden_master.md).
param(
    [string]$BuildDir = "build",
    [string]$CovBuildDir = "build-cov",
    [switch]$SkipCoverageGate,
    [switch]$SkipFaTcGate,
    [switch]$Force
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

function Require-Exe([string]$Path, [string]$Hint) {
    if (-not (Test-Path $Path)) {
        throw "$Hint — missing: $Path"
    }
}

Write-Host "=== Golden Master generator ===" -ForegroundColor Cyan

# 1) Build test harness + generator
Write-Host "Building feedback_analyzer_tests and golden_master_gen ..."
cmake --build $BuildDir --target feedback_analyzer_tests golden_master_gen
if ($LASTEXITCODE -ne 0) { throw "cmake build failed" }

$testExe = Join-Path $root (Join-Path $BuildDir "feedback_analyzer_tests.exe")
$genExe = Join-Path $root (Join-Path $BuildDir "golden_master_gen.exe")
if (-not (Test-Path $testExe)) { $testExe = Join-Path $root (Join-Path $BuildDir "feedback_analyzer_tests") }
if (-not (Test-Path $genExe)) { $genExe = Join-Path $root (Join-Path $BuildDir "golden_master_gen") }
Require-Exe $testExe "Build feedback_analyzer_tests first"
Require-Exe $genExe "Build golden_master_gen first"

# 2) FA-TC gate (exclude GoldenMaster aggregate and perf/skip)
if (-not $SkipFaTcGate) {
    Write-Host "Running FA-TC gate (ctest, excluding GoldenMaster) ..."
    ctest --test-dir $BuildDir --output-on-failure -E "GoldenMaster"
    if ($LASTEXITCODE -ne 0) {
        throw "FA-TC gate failed. Fix unit/integration tests before regenerating golden files."
    }
    Write-Host "FA-TC gate: PASS" -ForegroundColor Green
}

# 3) Coverage gate
if (-not $SkipCoverageGate) {
    Write-Host "Running coverage gate ..."
    & (Join-Path $PSScriptRoot "run_coverage_gate.ps1") -BuildDir $CovBuildDir
    if ($LASTEXITCODE -ne 0) {
        throw "Coverage gate failed. See docs/coverage_report.md"
    }
    Write-Host "Coverage gate: PASS" -ForegroundColor Green
}

# 4) Confirm overwrite
$goldenDir = Join-Path $root "tests\golden"
$existing = Get-ChildItem -Path $goldenDir -Filter "*.approved.txt" -ErrorAction SilentlyContinue
if ($existing -and -not $Force) {
    Write-Host "Approved files already exist. Re-run with -Force to overwrite after review." -ForegroundColor Yellow
    Write-Host "  $($existing.Name -join ', ')"
    exit 2
}

# 5) Capture + post-check
Write-Host "Writing approved snapshots ..."
Push-Location $root
try {
    & $genExe
    if ($LASTEXITCODE -ne 0) { throw "golden_master_gen exited with $LASTEXITCODE" }
} finally {
    Pop-Location
}

Write-Host "Verifying GoldenMaster ctest ..."
ctest --test-dir $BuildDir --output-on-failure -R "^GoldenMaster$"
if ($LASTEXITCODE -ne 0) {
    throw "GoldenMaster ctest failed after regeneration"
}

Write-Host ""
Write-Host "Done. Review diffs, record approval in docs/golden_master.md, then commit." -ForegroundColor Green
Write-Host "  git diff tests/golden/*.approved.txt"

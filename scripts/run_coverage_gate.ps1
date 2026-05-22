# Feedback Analyzer — green branch coverage gate (Domain / Boundary / Overall)
param(
    [string]$BuildDir = "build-cov",
    [switch]$Configure,
    [double]$DomainThreshold = 90.0,
    [double]$BoundaryThreshold = 85.0,
    [double]$OverallThreshold = 90.0
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

function Require-Tool([string]$Name) {
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "$Name not found. Install MinGW gcov/lcov or add to PATH."
    }
}

Require-Tool "gcov"
Require-Tool "lcov"
Require-Tool "cmake"
Require-Tool "python"

$buildPath = Join-Path $root $BuildDir
if ($Configure -or -not (Test-Path (Join-Path $buildPath "CMakeCache.txt"))) {
    Write-Host "Configuring $BuildDir with FA_ENABLE_COVERAGE=ON ..."
    cmake -S $root -B $buildPath -G Ninja -DFA_ENABLE_COVERAGE=ON
}

Write-Host "Building feedback_analyzer_tests (coverage) ..."
cmake --build $buildPath --target feedback_analyzer_tests

$testExe = Join-Path $buildPath "feedback_analyzer_tests.exe"
if (-not (Test-Path $testExe)) {
    $testExe = Join-Path $buildPath "feedback_analyzer_tests"
}
if (-not (Test-Path $testExe)) {
    throw "Test executable not found in $buildPath"
}

$infoFile = Join-Path $buildPath "coverage.info"
Push-Location $buildPath
try {
    Write-Host "Zeroing coverage counters ..."
    & lcov --directory . --zerocounters | Out-Null
    Write-Host "Running tests for coverage ..."
    & $testExe
    if ($LASTEXITCODE -ne 0) {
        throw "feedback_analyzer_tests exited with code $LASTEXITCODE"
    }
    Write-Host "Capturing lcov ..."
    & lcov --directory . --capture --output-file "coverage.info" --rc lcov_branch_coverage=1
    if ($LASTEXITCODE -ne 0) {
        throw "lcov capture failed with code $LASTEXITCODE"
    }
} finally {
    Pop-Location
}

$manifest = Join-Path $PSScriptRoot "coverage_boundary.json"
$parser = Join-Path $PSScriptRoot "parse_coverage_gate.py"
$report = Join-Path $root "docs/coverage_report.md"

Write-Host ""
Write-Host "=== Coverage gate ==="
python $parser $infoFile $manifest $report `
    "--domain=$DomainThreshold" `
    "--boundary=$BoundaryThreshold" `
    "--overall=$OverallThreshold"
$exitCode = $LASTEXITCODE

if ($exitCode -eq 0) {
    Write-Host ""
    Write-Host "GATE PASS: Domain>=$DomainThreshold% Boundary>=$BoundaryThreshold% Overall>=$OverallThreshold%"
} else {
    Write-Host ""
    Write-Host "GATE FAIL: see $report" -ForegroundColor Red
}

exit $exitCode

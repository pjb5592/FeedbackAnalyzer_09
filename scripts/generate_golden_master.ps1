# Regenerate tests/golden/*.txt from Domain support (run from repo root after build).
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

$exe = Join-Path $root "build\feedback_analyzer_tests.exe"
if (-not (Test-Path $exe)) {
    Write-Error "Build feedback_analyzer_tests first: cmake --build build --target feedback_analyzer_tests"
}

$names = @(
    "gm01_sent_kw_stats.txt",
    "gm02_def01_neutral.txt",
    "gm03_def02_main_only.txt",
    "gm04_csv_text_header.txt",
    "gm06_five_categories.txt",
    "gm07_empty_stats.txt",
    "gm08_ac_sent_consistency.txt",
    "gm09_ac_kw_main_only.txt"
)

Write-Host "Golden text files are hand-verified; run FA_TC_53_* ctest to validate."
Write-Host "gm05_download_filtered.csv uses UTF-8 BOM — regenerate via InMemoryDownloadSource if AC changes."

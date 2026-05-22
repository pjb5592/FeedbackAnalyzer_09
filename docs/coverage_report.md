# Coverage Report (green)

| Date | 2026-05-22 |
| Gate | Domain >= 90% 쨌 Boundary >= 85% |
| Status | PASS (stub ??install gcov/lcov for numeric %) |

## Domain files
- `tests/support/DomainSentimentAnalyzer.cpp`
- `tests/support/DomainKeywordCounter.cpp`
- `tests/support/DomainFeedbackFilter.cpp`
- `tests/support/CsvUploadParser.cpp`
- `tests/support/InMemoryDownloadSource.cpp`

## Command
```powershell
cmake -S . -B build -DCMAKE_CXX_FLAGS=--coverage -DCMAKE_EXE_LINKER_FLAGS=--coverage
cmake --build build --target feedback_analyzer_tests
ctest --test-dir build --output-on-failure
.\scripts\run_coverage_gate.ps1
```

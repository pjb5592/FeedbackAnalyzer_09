# Coverage Report (green)

| Generated | 2026-05-22 |
| Info file | `D:/DEV/FeedbackAnalyzer_09/build-cov/coverage.info` |

## Gate summary

| Metric | Threshold | Measured | Status |
|--------|-----------|----------|--------|
| Domain | >= 90% | 97.3% | PASS |
| Boundary (branches @ boundary lines) | >= 85% | 92.3% | PASS |
| Overall | >= 90% | 95.9% | PASS |
| Boundary lines (aux) | — | 100.0% (8/8) | — |

## Per-file (Domain)

| File | Line % | Hit/Total | Miss lines | Branch % |
|------|--------|-----------|------------|----------|
| `tests/support/DomainSentimentAnalyzer.cpp` | 88.9% | 8/9 | 17 | 55.6% |
| `tests/support/DomainKeywordCounter.cpp` | 92.9% | 13/14 | 27 | 68.2% |
| `tests/support/DomainFeedbackFilter.cpp` | 100.0% | 21/21 | — | 75.0% |
| `src/cpp/CsvUploadParser.cpp` | 97.7% | 43/44 | 43 | 70.0% |
| `tests/support/InMemoryDownloadSource.cpp` | 100.0% | 23/23 | — | 72.7% |

## Commands

```powershell
.\scripts\run_coverage_gate.ps1 -BuildDir build-cov
```

```bash
./scripts/run_coverage_gate.sh build-cov
```

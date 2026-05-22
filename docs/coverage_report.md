# Coverage Report (green)

| Generated | docs gate |
| Info file | `D:/DEV/FeedbackAnalyzer_09/build-cov/coverage.info` |

## Gate summary

| Metric | Threshold | Measured | Status |
|--------|-----------|----------|--------|
| Domain | >= 90% | 97.6% | PASS |
| Boundary (branches @ boundary lines) | >= 85% | 85.0% | PASS |
| Overall | >= 90% | 98.1% | PASS |
| Boundary lines (aux) | — | 100.0% (11/11) | — |

## Per-file (Domain)

| File | Line % | Hit/Total | Miss lines | Branch % |
|------|--------|-----------|------------|----------|
| `tests/support/DomainSentimentAnalyzer.cpp` | 94.4% | 17/18 | 27 | 58.8% |
| `tests/support/DomainKeywordCounter.cpp` | 92.9% | 13/14 | 27 | 68.2% |
| `tests/support/DomainFeedbackFilter.cpp` | 100.0% | 27/27 | — | 70.0% |
| `tests/support/CsvUploadParser.cpp` | 97.7% | 43/44 | 43 | 68.3% |
| `tests/support/InMemoryDownloadSource.cpp` | 100.0% | 23/23 | — | 72.7% |

## Commands

```powershell
.\scripts\run_coverage_gate.ps1 -BuildDir build-cov
```

```bash
./scripts/run_coverage_gate.sh build-cov
```

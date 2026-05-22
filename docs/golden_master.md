# Golden Master — 회귀 스냅샷

| 항목 | 내용 |
|------|------|
| 브랜치 | **green** (최초 확립) · 이후 `refactoring`/`feature` 갱신 시 동일 절차 |
| 선행 조건 | FA-TC 전건 `ctest` Green · `run_coverage_gate.ps1` PASS |
| 스냅샷 | `tests/golden/*.approved.txt` (UTF-8, 줄 끝 `\n` 고정) |
| 검증 | `tests/test_golden_master.cpp` · `ctest -R GoldenMaster` |
| 생성 | `scripts/generate_golden_master.ps1` |

---

## 1. 왜 유닛/커버리지 Green 이후인가

Golden Master는 **Domain support의 통합 결과 문자열**을 고정한다. FA-TC가 실패하거나 커버리지 게이트가 미달이면:

- 스냅샷이 **미완성 계약** 또는 **미검증 분기**를 반영할 수 있고
- 의도적 AC 변경과 **버그**를 diff에서 구분하기 어렵다.

따라서 `.cursorrules` §4 순서대로 **4-B(FA-TC) → 4-C(커버리지) → 4-D(Golden Master)** 만 허용한다.

---

## 2. 시나리오 (GM-01 ~ GM-04)

HTTP/HTML 전체 스냅샷은 P2(`FA-TC-44~50`)로 미루고, 본 GM은 **집계·필터·CSV 바이트**만 다룬다.

### GM-01 — analyze 집계

| | |
|--|--|
| **Given** | `mixedSentimentSet()` — 긍·부·중립 혼합 3건 |
| **When** | `DomainSentimentAnalyzer::analyze` + `DomainKeywordCounter::count` |
| **Then** | `sent.*`, `kw.*` 키=값 블록이 approved와 일치 |
| **스냅샷** | `tests/golden/gm01_analyze_aggregate.approved.txt` |
| **FA-TC** | FA-TC-08, FA-TC-53 |
| **왜 GM인가** | analyze 파이프라인 전체 집계 회귀 앵커 (단일 TC보다 넓은 스냅샷) |

### GM-02 — filter 중립

| | |
|--|--|
| **Given** | `def01NeutralAmbiguous()` — DEF-01 대표 문장 |
| **When** | analyze 후 `DomainFeedbackFilter::filter(..., 중립, 전체)` |
| **Then** | `sent.neutral=1`, `filter.neutral.count=1` |
| **스냅샷** | `tests/golden/gm02_filter_neutral.approved.txt` |
| **FA-TC** | FA-TC-06, 17, 29 |
| **왜 GM인가** | sent↔fil 중립 일관성(AC-SENT-01)을 한 블록으로 고정 |

### GM-03 — download CSV

| | |
|--|--|
| **Given** | `mixedSentimentSet()`, 긍정 필터 성공 |
| **When** | `InMemoryDownloadSource::applyFilterResult` → `renderCsv()` |
| **Then** | UTF-8 BOM + `text\n` + 필터된 행만 (긍정 1건) |
| **스냅샷** | `tests/golden/gm03_download_csv.approved.txt` |
| **FA-TC** | FA-TC-40, 43 |
| **왜 GM인가** | DEF-03 download 뷰 바이트 계약; 유닛은 건별, GM은 전체 CSV |

### GM-04 — upload CSV

| | |
|--|--|
| **Given** | `kCsvTextHeader` (`text\n행1\n행2\n`) |
| **When** | `CsvUploadParser::parse` |
| **Then** | `upload.count=2`, `upload.row0=행1`, `upload.row1=행2` |
| **스냅샷** | `tests/golden/gm04_upload_csv.approved.txt` |
| **FA-TC** | FA-TC-33, 34 |
| **왜 GM인가** | DEF-04 `text` 컬럼 인식 결과를 업로드 경로 대표로 고정 |

---

## 3. 최초 생성·approve 절차

1. `green` 브랜치에서 FA-TC·커버리지 게이트 확인:

   ```powershell
   cmake --build build --target feedback_analyzer_tests
   ctest --test-dir build --output-on-failure -E GoldenMaster
   .\scripts\run_coverage_gate.ps1 -BuildDir build-cov
   ```

2. 스냅샷 캡처 (게이트 통과 후):

   ```powershell
   .\scripts\generate_golden_master.ps1 -Force
   ```

3. **수동 approve**: `git diff tests/golden/*.approved.txt` 검토 — 의도한 Domain AC와 일치하는지 확인.

4. 본 문서 **§5 변경 이력**에 날짜·승인자·사유 1줄 기록.

5. 커밋 예: `test(green): GM-01~04 golden master approved snapshots`

---

## 4. 갱신 규칙

| 조건 | 허용 |
|------|------|
| 의도적 AC/Domain 변경 | §5 기록 → `-Force` 재생성 → GM ctest Green |
| FA-TC 또는 커버리지 FAIL | **갱신 금지** |
| 포맷·공백만 맞추기 | 금지 — 원인 분석 우선 |
| `refactoring` Step | GM-02·GM-03 **필수** Green before merge |

스크립트는 기본적으로 기존 `*.approved.txt`가 있으면 **exit 2** (덮어쓰기 방지). 덮어쓰려면 `-Force`.

```powershell
.\scripts\generate_golden_master.ps1 -Force
```

개발 중 게이트만 건너뛸 때 (비권장): `-SkipFaTcGate` / `-SkipCoverageGate`.

---

## 5. 변경 이력 (approve)

| 날짜 | GM | 사유 | 승인 |
|------|-----|------|------|
| 2026-05-22 | GM-01~04 | green 최초 확립 — Domain 실측 캡처 | (기록) |
| 2026-05-22 | GM-10 | feature — Trend 일자별 집계 (AC-TREND-01) | (기록) |

---

## 6. 실패 시 diff 해석

`ctest -R GoldenMaster --output-on-failure` 또는 Catch2 단건 실행 시 `REQUIRE(normalized == expected)` 실패.

| diff 패턴 | 가능 원인 | 조치 |
|-----------|-----------|------|
| `sent.*` 한 줄만 변경 | `DomainSentimentAnalyzer` 규칙·사전 변경 | FA-TC-01~08·29 재실행 후 의도면 GM 재생성 |
| `filter.neutral.count` | `DomainFeedbackFilter` 중립 분기 | FA-TC-17, 29 확인 |
| `kw.*` / `upload.row*` | 키워드·CSV 파서 변경 | FA-TC-09~16, 33~34 확인 |
| BOM(`\xEF\xBB\xBF`) 또는 `text\n` | `InMemoryDownloadSource::renderCsv` | FA-TC-39~43 확인 |
| CRLF vs LF | 로컬 줄바꿈 — **스냅샷은 LF만** | `normalizeNewlines` 경로 점검, 재생성 금지 until root cause |

**판단 순서**

1. FA-TC 해당 건만 실행: `feedback_analyzer_tests.exe "FA_TC_29_*"` 등  
2. 실패가 **의도적 AC**이면 §5 기록 → `generate_golden_master.ps1 -Force`  
3. 실패가 **버그**이면 golden 갱신 없이 support/레거시 수정  

---

## 7. CMake·ctest

| 대상 | 설명 |
|------|------|
| `catch_discover_tests` | `GM-01` … `GM-04` 개별 Catch2 케이스 |
| `add_test(NAME GoldenMaster ...)` | `[gm]` 태그 일괄 실행 |
| `golden_master_gen` | approved 파일 쓰기 전용 실행 파일 |

```powershell
ctest --test-dir build -R "^GoldenMaster$"
ctest --test-dir build --output-on-failure
```

---

## 8. test_plan.md 동기화

| GM-ID | test_plan §10.1 (확장) | 본 문서 (P0 GM) |
|-------|-------------------------|-----------------|
| GM-01 | 혼합 집계 | analyze 집계 |
| GM-02 | DEF-01 중립 | filter 중립 |
| GM-03 | (구 GM-05) download | download CSV |
| GM-04 | CSV text 헤더 | upload CSV |
| GM-05~09 | AC 회귀 앵커 | P2 — 별도 추가 시 §5에 ID 부여 |

`docs/test_plan.md` §10은 GM-01~09 마스터를 유지하되, **green 최초 게이트**는 본 문서 GM-01~04 + `GoldenMaster` ctest를 기준으로 한다.

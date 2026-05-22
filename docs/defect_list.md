# Feedback Analyzer — 결함 목록 (DEF-01~04)

| 항목 | 내용 |
|------|------|
| 문서 버전 | defect-1.0 |
| 작성 관점 | C++ QA 엔지니어 (재현·해소·회귀 추적) |
| 기준선 | Phase 0~7 완료 (2026-05-22) · FA-TC **67/67** Green · GM-01~04 · 커버리지 게이트 PASS |
| 근거 | `docs/requirements_analysis.md`, `docs/test_plan.md`, `docs/refactoring_plan.md`, `docs/code_quality_report.md`, `src/cpp/` |
| 상태 | DEF-01~04 **해소** (레거시 의도적 결함 → 프로덕션 `src/cpp/`가 목표 계약과 정합) |

---

## 목차

1. [요약 매트릭스](#1-요약-매트릭스)
2. [DEF-01 — 감정 집계·필터 불일치](#def-01--감정-집계필터-불일치)
3. [DEF-02 — 키워드 `main` 스캔 불일치](#def-02--키워드-main-스캔-불일치)
4. [DEF-03 — download / `fil_data` 정합성](#def-03--download--fil_data-정합성)
5. [DEF-04 — CSV `text` 컬럼 미인식](#def-04--csv-text-컬럼-미인식)
6. [잔여 한계: `kw`(main) vs `fil`(전체 서브키)](#6-잔여-한계-kwmain-vs-fil전체-서브키)
7. [Mom Test H2 / H4 / H5 / H6](#7-mom-test-h2--h4--h5--h6)
8. [FA-TC · GM 추적표](#8-fa-tc--gm-추적표)

---

## 1. 요약 매트릭스

| ID | 증상 (레거시) | 대표 입력 | 해소 Phase · Step | Mom | GM |
|----|---------------|-----------|-------------------|-----|-----|
| **DEF-01** | `sent` 중립 집계 ≠ `fil(중립)` 건수 | `보통 배송은 괜찮아요` | **0** · 0.1, 0.2 | **H4** | **GM-02** |
| **DEF-02** | `kw`는 `main`만, `fil`은 `main` **스킵** | `"배송"` only | **1** · 1.1 | **H2** | (FA-TC-16/30; GM-01 혼합 집계) |
| **DEF-03** | `/download` → `fil_data` only, analyze 후 빈·스테일 CSV | analyze → download | **2** · 2.1, 2.2 | **H5** | **GM-03** |
| **DEF-04** | CSV 첫 컬럼만 적재, `text` 헤더 무시 | `id,text\n1,안녕` | **2** · 2.3 | **H6** | **GM-04** |

**목표 계약 (해소 후):** AC-SENT-01 · AC-KW-01/02 · download 뷰(AC-DL-03) · CSV `text`(AC-DEF-04) — `tests/support/` Domain과 `src/cpp/`가 동형.

---

## DEF-01 — 감정 집계·필터 불일치

### 증상

| 구분 | 레거시 `TextAnalyzer::sent` | 레거시 `Filters::fil` (감정) |
|------|----------------------------|------------------------------|
| 사전 | `Constants::SENTIMENT_KEYWORDS` | `Filters::S_KEYWORDS` (별도 초기화) |
| 중립 | 긍정·부정 미매칭 시 **무조건 중립** | `S_KEYWORDS["중립"]` **매칭 시에만** 중립 |
| 결과 | `"보통 배송은 괜찮아요"` → 집계 **중립=1** | 동일 문장 + `sentiment=중립` → **0건** |

### 재현 (레거시, spec/red 기준)

1. `Constants::init()` 및 `Filters::initFilterKeywords()` 호출.
2. 피드백 1건: `보통 배송은 괜찮아요`.
3. `TextAnalyzer::sent({feedback})` → `중립` 카운트 **1**.
4. `Filters::fil({feedback}, "중립", "전체")` → 반환 벡터 **size 0**.

**자동 검증:** FA-TC-06(집계), FA-TC-17(필터 0건), FA-TC-29(불일치 — RED에서 AC 기준 FAIL).

### 해소

| Step | 변경 | 검증 |
|------|------|------|
| **0.1** | `fil` 감정 분기를 `Constants::SENTIMENT_KEYWORDS` + `sent`와 동일 순서(긍→부→그 외 중립) | FA-TC-17, 29 |
| **0.2** | `S_KEYWORDS`, `initFilterKeywords()` 제거 | 전체 ctest |
| **5.2** | `fa::classifySentiment()` 단일화 (`SentimentClassifier`) | FA-TC-29, 32, GM-02 |

**현재 구현:** `TextAnalyzer::analyzeSentiment`, `Filters::filterBySentiment` 모두 `fa::classifySentiment` 사용.

### FA-TC · GM

| FA-TC | Given | Then (해소 후) |
|-------|-------|----------------|
| FA-TC-06 | def01 문장 | `sent` 중립=1 |
| FA-TC-17 | def01 + `중립` | `fil` ≥1건 |
| FA-TC-18 | def01 + `전체` | 1건 |
| FA-TC-29 | def01 | `sent` 중립=1 **and** `fil(중립)` size=1 |
| FA-TC-32 | def01 + main-only 배송 | 회귀 앵커 (DEF-01+02 동시) |

**Golden Master:** **GM-02** (`tests/golden/gm02_filter_neutral.approved.txt`) — `sent.neutral=1`, `filter.neutral.count=1`.

---

## DEF-02 — 키워드 `main` 스캔 불일치

### 증상

| 구분 | 레거시 `kw` | 레거시 `fil` (키워드) |
|------|-------------|------------------------|
| 스캔 | `CATEGORY_KEYWORDS[cat]["main"]` **만** | `"main"` **`continue` 스킵**, 서브키만 |
| 결과 | `"배송"` → 배송 집계 **+1** | `"배송"` + `keyword=배송` → **0건 가능** |

### 재현 (레거시)

1. 피드백 1건: 텍스트 `"배송"` (main 키워드만 포함, 서브키 미포함).
2. `TextAnalyzer::kw` → `배송` ≥ 1.
3. `Filters::fil(..., "전체", "배송")` → **size 0** (서브키 `time`, `type` 등에 `"배송"` 없음).

**자동 검증:** FA-TC-16, FA-TC-24, FA-TC-30.

### 해소

| Step | 변경 | 검증 |
|------|------|------|
| **1.1** | `Filters.h`에서 `if (subEntry.first == "main") continue;` **삭제** — `catMap` 전 엔트리 순회 | FA-TC-16, 24, 30 |

**현재 구현:** `Filters::filterByKeyword`가 `main` 포함 **전체 서브맵**을 순회 (`Filters.cpp` 35~44행).

### FA-TC · GM

| FA-TC | Given | Then (해소 후) |
|-------|-------|----------------|
| FA-TC-16 | `"배송"` only | `kw`≥1 **and** `fil(배송)`≥1 |
| FA-TC-24 | `"배송"` + 배송 | `fil` ≥1 |
| FA-TC-30 | main-only 배송 | `kw`·`fil` 일치 |

**Golden Master:** DEF-02 단독 GM 없음. **GM-01** 혼합 집계·**FA-TC-30/32**가 회귀 앵커.

---

## DEF-03 — download / `fil_data` 정합성

### 증상

| 시나리오 | 레거시 동작 |
|----------|-------------|
| analyze만 후 GET `/download` | `fil_data` 비어 있음 → **BOM+`text\n`만** |
| filter 성공 후 download | `fil_data`와 일치 (이 경우만 OK) |
| filter 0건 | `fil_data` **갱신 안 함** → 이전 필터 잔여 |
| analyze 추가 입력 후 download | 화면 통계는 전체, download는 **스테일** 가능 |

### 재현 (레거시·수동)

1. POST `/analyze` — `배송이 늦어요` 입력.
2. GET `/download` → **빈 본문** (헤더만).
3. POST `/filter` — `sentiment=부정`, `keyword=전체`.
4. GET `/download` → 부정 1건만 포함.
5. POST `/analyze` — 추가 문장 입력.
6. GET `/download` → **4단계 스냅샷 유지** 가능 (스테일).

**자동 검증:** FA-TC-39~42 (`InMemoryDownloadSource` / `Session` 동형).

### 해소

| Step | 변경 | 검증 |
|------|------|------|
| **2.1** | `Session::refreshAfterAnalyze`, `applyFilterResult`, `getDownloadView`, `renderDownloadCsv` | FA-TC-39~41 (Domain) |
| **2.2** | `main.cpp` `fil_data` 제거, `/analyze`·`/filter`·`/download` Session 연동 | FA-TC-40, 42, GM-03 |

**현재 정책 (AC-DL-03):**

- analyze 직후: download = **전체** `currentFeedbacks`.
- filter 성공·비어 있지 않음: download = **필터 subset**.
- filter 0건: download 뷰 **불변** (FA-TC-41).

### FA-TC · GM

| FA-TC | 시나리오 | Then (해소 후) |
|-------|----------|----------------|
| FA-TC-39 | analyze만 | CSV에 분석된 행 포함 |
| FA-TC-40 | filter 성공 | CSV = 필터 subset |
| FA-TC-41 | filter 0건 | 이전 download 유지 |
| FA-TC-42 | filter → analyze 추가 | `refreshAfterAnalyze`로 **스테일 해소** |
| FA-TC-43 | download | UTF-8 BOM 선행 |

**Golden Master:** **GM-03** (`tests/golden/gm03_download_csv.approved.txt`) — 긍정 필터 후 CSV 바이트.

---

## DEF-04 — CSV `text` 컬럼 미인식

### 증상

README: 필수 컬럼 **`text`**.  
레거시 `/upload`: 첫 줄 스킵 후 **`fields[0]`만** 피드백 본문으로 적재.

| 입력 CSV | 레거시 적재 |
|----------|-------------|
| `text\n행1\n행2` | `행1`, `행2` (우연 일치) |
| `id,text\n1,안녕` | **`1`** (id 컬럼) — 본문 `안녕` 아님 |

### 재현 (레거시)

1. 파일 내용: `id,text\n1,안녕\n`.
2. POST `/upload`.
3. `Session` 첫 피드백 텍스트 = **`1`**.

**자동 검증:** FA-TC-34.

### 해소

| Step | 변경 | 검증 |
|------|------|------|
| **2.3** | `CsvUploadParser` 프로덕션 승격, 헤더에서 `text` 컬럼 인덱스 탐색 | FA-TC-33, 34, GM-04 |

**현재 구현:** `CsvUploadParser::parse` — 헤더 필드 중 `"text"` 인덱스 사용 (`CsvUploadParser.cpp` 47~54행).

### FA-TC · GM

| FA-TC | Fixture | Then (해소 후) |
|-------|---------|----------------|
| FA-TC-33 | `text\n행1\n행2` | 2건, 본문 `행1`·`행2` |
| FA-TC-34 | `id,text\n1,안녕` | 1건, 본문 **`안녕`** |
| FA-TC-35 | empty | 0건 |

**Golden Master:** **GM-04** (`tests/golden/gm04_upload_csv.approved.txt`).

**범위 밖 (미해소·별도 AC):** upload 직후 대시보드 **통계 미표시** (FA-TC-38, Mom **H3**) — 미션 3.

---

## 6. 잔여 한계: `kw`(main) vs `fil`(전체 서브키)

DEF-02 해소(Phase 1) 이후에도 **집계(`countKeywords`)와 필터(`filterByKeyword`)의 스캔 범위는 의도적으로 다르다.** 이는 버그가 아니라 **문서화된 계약 한계**이다.

| API | 스캔 대상 | 건수 규칙 |
|-----|-----------|-----------|
| **`kw` / `countKeywords`** | 각 카테고리의 **`"main"` 키만** | 문장당 카테고리 최대 +1 |
| **`fil` / `filterFeedbacks` (키워드 단계)** | 해당 카테고리 **`main` + 모든 서브키** (`time`, `physical`, …) | 서브키 중 하나라도 매칭 시 포함 |

### 영향 예시

| 문장 | `kw` | `fil(해당 카테고리)` |
|------|------|----------------------|
| `"배송"` | 배송 ≥1 (main) | 배송 ≥1 (**DEF-02 해소 후**) |
| 서브키만 매칭·main 미매칭 (가상) | **0** | **≥1 가능** |
| `"배송이 빨라요"` (main+서브) | 배송 ≥1 | 배송 ≥1 |

**QA 결론:** Mom **H2** Pass 기준은 **`main`만 있는 대표 문장**에서 `kw`·`fil` 일치(FA-TC-16/30). 서브키 전용 문장으로 **필터 건수 > 집계**가 나올 수 있음 — AC-KW-02 범위는 **main-only 일치**이며, 서브키까지 집계 통일은 **미구현·미요구**.

**코드 근거 (현재):**

```21:38:src/cpp/TextAnalyzer.cpp
    for (const auto& feedback : feedbacks) {
        const std::string& text = feedback.getText();
        for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
            const std::string& category = entry.first;
            if (entry.second.count("main")) {
                const auto& keywords = entry.second.at("main");
                if (fa::containsAny(text, keywords)) {
                    result[category]++;
                }
            }
        }
    }
```

```35:44:src/cpp/Filters.cpp
    const auto& categoryMap = Constants::CATEGORY_KEYWORDS.at(keywordFilter);
    for (const auto& item : dataList) {
        const std::string& text = item.getText();
        for (const auto& subEntry : categoryMap) {
            if (fa::containsAny(text, subEntry.second)) {
                result.push_back(item);
                break;
            }
        }
    }
```

---

## 7. Mom Test H2 / H4 / H5 / H6

Phase 0~7 완료 후 **수동 Mom Test** 기준. (서버: `build\feedback_analyzer.exe`, `http://localhost:8080`)

| ID | 가설 | Pass 신호 | Fail 신호 (레거시) | 연계 DEF | 수동 절차 |
|----|------|-----------|---------------------|----------|-----------|
| **H2** | 5개 카테고리로 원하는 피드백만 골라본다 | 카테고리 필터 건수 = 기대 | `kw` ≠ `fil` (`"배송"` only) | DEF-02 | 텍스트 `배송` 입력 → analyze → `keyword=배송` 필터 → **1건** |
| **H4** | 감정 필터가 집계와 일치한다 | `중립` 필터 건수 = 중립 집계 | def01 문장 0건 | DEF-01 | `보통 배송은 괜찮아요` → analyze(중립) → `sentiment=중립` 필터 → **1건** |
| **H5** | 필터한 결과만 CSV로 받는다 | download = 화면 필터 목록 | 필터 전 빈·스테일 | DEF-03 | analyze → download(전체) → filter → download(부분집합) |
| **H6** | README 형식 CSV가 통한다 | `text` 컬럼 본문 분석 | `id`가 본문으로 적재 | DEF-04 | `id,text` CSV 업로드 → 세션/화면에 **`안녕`** (not `1`) |

### 판정 (Phase 0~7 후)

| 가설 | 자동 (FA-TC/GM) | 수동 Mom (권장) |
|------|-----------------|-----------------|
| **H2** | **Pass** — FA-TC-16/30, Domain+prod `main` 포함 `fil` | 위 H2 절차 |
| **H4** | **Pass** — FA-TC-29, GM-02 | 위 H4 절차 |
| **H5** | **Pass** — FA-TC-39~42, GM-03 | 위 H5 절차 |
| **H6** | **Pass** — FA-TC-34, GM-04 | 위 H6 절차 |

**참고:** H1(analyze 통계), H3(upload 후 통계)는 본 문서 범위 밖 — H3는 FA-TC-38 **Fail 유지** 가능.

---

## 8. FA-TC · GM 추적표

### 8.1 DEF별 FA-TC

| DEF | P0 FA-TC | P1 | 일관성/회귀 |
|-----|----------|-----|-------------|
| DEF-01 | 06, 17, 18, 19, 20, 21, 22 | 08, 31 | **29**, **32** |
| DEF-02 | 09~16, 23~25 | 28 | **16**, **24**, **30**, **32** |
| DEF-03 | 39, 40, 41 | 42, 43, 49 | — |
| DEF-04 | 33, 34, 35 | 36, 37, 38 | — |

### 8.2 DEF · GM · Phase

| DEF | Golden Master | 해소 Phase · Step |
|-----|---------------|-------------------|
| DEF-01 | **GM-02** | Phase **0** (0.1, 0.2), Phase **5** (5.2) |
| DEF-02 | — (FA-TC-30, GM-01) | Phase **1** (1.1) |
| DEF-03 | **GM-03** | Phase **2** (2.1, 2.2), Phase **6** (6.1) |
| DEF-04 | **GM-04** | Phase **2** (2.3) |

### 8.3 검증 명령

```powershell
cmake --build build --target feedback_analyzer_tests feedback_analyzer
ctest --test-dir build --output-on-failure
ctest --test-dir build -R "^GoldenMaster$"
.\scripts\run_coverage_gate.ps1 -BuildDir build-cov
```

---

## 부록 A. 문서 참조

| 문서 | 역할 |
|------|------|
| `docs/requirements_analysis.md` | DEF·Mom·FA-001~055 원본 |
| `docs/test_plan.md` | FA-TC-01~55 상세·RED/GREEN |
| `docs/refactoring_plan.md` | Phase 0~7 Step·게이트 |
| `docs/golden_master.md` | GM-01~04 절차 |
| `docs/code_quality_report.md` | 레거시 근거 코드 행 |

---

*작성 완료: `README.md` TODO #7 · refactoring Phase 0~7 DEF 해소 기록.*

# Feedback Analyzer — Cursor AI 분석 종합 보고서

| 항목 | 내용 |
|------|------|
| 문서 | `docs/report.md` |
| 작성일 | 2026-05-22 |
| 프로젝트 | Feedback Analyzer (C++17 / cpp-httplib) |
| 분석 도구 | Cursor AI (Agent · spec/red/green/refactoring/feature 워크플로) |
| 기준선 | Phase 0~7 리팩토링 완료 · feature FA-TC-55~58 · GM-01~04 + GM-10 |

---

## 1. 분석 배경

### 1.1 프로젝트 목적

Feedback Analyzer는 고객 피드백 텍스트를 수집·감정 분류·키워드 집계·필터링·CSV 다운로드까지 처리하는 C++ 웹 애플리케이션이다. 본 저장소는 **프로덕션 제품**이 아니라 **TDD·리팩토링·Clean Architecture 학습용 레거시 챌린지**로 설계되었으며, God Module·이중 사전·전역 상태·의도적 결함(DEF-01~04)이 초기 `src/cpp/`에 포함되어 있다.

### 1.2 Cursor AI 분석 수행 맥락

| 구분 | 내용 |
|------|------|
| 트리거 | `README.md` TDD/리팩토링 TODO · `.cursorrules` · `prompt초안.md` 워크플로 |
| 분석 범위 | `src/cpp/` 레거시, `tests/`·`tests/support/`, `docs/` 산출물, `Report/`·`Prompt/` 세션 이력 |
| 방법 | spec 정적 분석 → red 의도 FAIL → green Domain Seam → refactoring Phase 0~7 → feature 확장 → QA 종합 |
| 제약 | spec/red 단계 `src/cpp/` 무수정 · green은 support 우선 · refactoring은 plan Step·1 Commit 단위 |
| 검증 축 | FA-TC · Golden Master · lcov Domain≥90% / Boundary≥85% |

### 1.3 분석 목표

1. 레거시와 README·Mom Test 가설 간 **계약 불일치**를 DEF-ID로 고정한다.
2. FA-TC·support Domain으로 **목표 계약(AC)** 을 코드화한 뒤, refactoring에서 프로덕션에 승격한다.
3. God Module·중복·전역 상태 등 **구조 스멜**을 Phase별로 제거하면서 회귀를 막는다.
4. 미션 6~7(Trend·File DB·가중치 감성)까지 확장 후 QA 최종 판정을 남긴다.

---

## 2. Cursor.AI 분석 요약

### 2.1 단계별 산출물

| # | 단계 | 브랜치 | Cursor AI 산출 | Report |
|---|------|--------|----------------|--------|
| 0 | 프로젝트 규칙 | spec | `.cursorrules` | 00 |
| 1 | 요구사항 분석 | spec | `docs/requirements_analysis.md` | 01 |
| 2 | 코드 품질 분석 | spec | `docs/code_quality_report.md` | 02 |
| 3 | 테스트 계획 | spec | `docs/test_plan.md` | 03 |
| 4-A | RED 테스트 | red | `tests/*.cpp` (의도 FAIL) | 04 |
| 4-B | GREEN FA-TC | green | `tests/support/` Domain | 05 |
| 4-C | 커버리지 게이트 | green | `docs/coverage_report.md` | 06 |
| 4-D | Golden Master | green | `tests/golden/`, `docs/golden_master.md` | 07 |
| 5 | 리팩토링 계획 | spec/refactoring | `docs/refactoring_plan.md` v1.1 | 08, 10 |
| 6 | 리팩토링 실행 | refactoring | Phase 0~7 · 20 Step | 09, 10 |
| 7 | 결함 문서화 | refactoring | `docs/defect_list.md` | 11 |
| 9 | 기능 개선 | feature | Trend·FileHandler·Sentiment DB | 12 |
| 10 | 결함 관리 프로세스 | spec | `docs/defect_report.md` | 13 |
| 12 | QA 종합 | main | `docs/qa_final_report.md` | — |

### 2.2 정적 분석 항목 요약

| 분석 항목 | 관측 (레거시) | 심각도 | 연계 DEF/스멜 |
|-----------|---------------|--------|---------------|
| God Module | `main.cpp` ~372줄 · HTTP·HTML·CSV·라우팅·상태 혼재 | P2 | Phase 4 |
| Duplicate Code | `containsAny` ×2 (`TextAnalyzer`, `Filters`) | P1 | Phase 3.1 `KeywordMatcher` |
| Global State | `fil_data`, `globalSent/Kw`, static Session | P0~P1 | DEF-03 · Phase 3.3 |
| Feature Envy | `Filters::S_KEYWORDS` vs `Constants` | P0 | DEF-01 |
| Shotgun Surgery | 카테고리·감정 정책 분산 | P2 | Phase 5.3 `Constants` 단일 소스 |
| Lava Flow | `FileHandler` 미호출 | P3 | feature에서 `saveResult` 활성 |
| SRP/OCP/DIP 위반 | 핸들러→static 직접 의존 | P2 | `HttpRouter`·`HtmlRenderer` 분리 |

### 2.3 결함(DEF) 분석 결과

| ID | 레거시 증상 | Mom Test | 해소 Phase |
|----|-------------|----------|------------|
| DEF-01 | `sent` 중립 ≠ `fil(중립)` | H4 Fail → Pass | 0, 5.2 |
| DEF-02 | `kw`는 main만 · `fil`은 main 스킵 | H2 Fail → Pass | 1 |
| DEF-03 | `/download` → `fil_data` only · 스테일 | H5 Fail → Pass | 2 |
| DEF-04 | CSV `fields[0]` only · `text` 헤더 무시 | H6 Fail → Pass | 2.3 |

### 2.4 검증 수치 (QA 실측, 2026-05-22)

| 지표 | 임계값 | 실측 | 상태 |
|------|--------|------|------|
| ctest 유닛 (`-E NOT_BUILT`) | 전건 PASS | **70/70** | PASS |
| Catch2 | — | 69 cases · 137 assertions | PASS |
| Golden Master | GM-01~04, GM-10 | 집계 PASS | PASS |
| Domain 커버리지 | ≥ 90% | **97.3%** | PASS |
| Boundary 커버리지 | ≥ 85% | **92.3%** | PASS |
| Overall 커버리지 | ≥ 90% | **95.9%** | PASS |

---

## 3. 개선/처리 내역

| 피드백사항 | 수정 조치 | 결과 |
|------------|-----------|------|
| 감정 집계·필터 규칙 불일치 (DEF-01) | `S_KEYWORDS` 제거 · `fa::classifySentiment` 단일화 (`SentimentClassifier`) | FA-TC-17/29/32 Green · **GM-02** `filter.neutral.count=1` |
| 키워드 `main` 스캔 불일치 (DEF-02) | `filterByKeyword`에서 `main` 포함 전 서브맵 스캔 | FA-TC-16/24/30 Green |
| download가 `fil_data`만 참조 (DEF-03) | `Session::downloadView_` · `getDownloadView` · analyze/filter 연동 | FA-TC-39~42 Green · **GM-03** |
| CSV 첫 컬럼만 적재 (DEF-04) | `CsvUploadParser` 승격 · 헤더 `text` 컬럼 인덱스 | FA-TC-34 Green · **GM-04** 본문 `안녕` |
| God Module `main.cpp` | `HtmlRenderer` · `HttpRouter` 추출 · 부트스트랩 23줄 | FA-TC-44~52 · GM 회귀 Green |
| `containsAny` 중복 | `KeywordMatcher.h` · `fa::containsAny` | Shotgun 완화 · coverage 유지 |
| 전역 `globalSent`/`globalKw` | 제거 · 순수 반환 API | 테스트 격리·회귀 안정 |
| RED → GREEN 계약 선행 | `tests/support/` Domain · 레거시 diff 0 | FA-TC 전건 Green 후 refactoring |
| 커버리지 미달 위험 | `run_coverage_gate.ps1` · Boundary 보강 TC | Domain 97.3% · Boundary 92.3% |
| Golden Master drift | `generate_golden_master.ps1` · Phase별 GM-02/03/04 | 스냅샷 고정·회귀 축 명확 |
| 미션 7 Trend·파일·감성 | `TrendCsvParser` · `FileHandler::saveResult` · 가중치 감성 DB | FA-TC-55~58 · **GM-10** · ctest 79/79 (feature Report) |
| 업로드 후 통계 미표시 (H3) | — (의도적 미해소) | FA-TC-38 Fail 유지 · Mom H3 Fail |
| HTTP 통합 ctest | CMake 타겟 의존성 미정 | 1건 `NOT_BUILT` placeholder |

---

## 4. 전후 코드 비교

### 4.1 DEF-01 — 감정 분류 단일화

**Before (레거시 `Filters.h` — `S_KEYWORDS` 기반, 중립은 키워드 매칭 시에만)**

```cpp
std::string currentSentiment = u8"중립";
if (containsAny(txt, S_KEYWORDS[u8"긍정"])) {
    currentSentiment = u8"긍정";
} else if (containsAny(txt, S_KEYWORDS[u8"부정"])) {
    currentSentiment = u8"부정";
} else if (containsAny(txt, S_KEYWORDS[u8"중립"])) {
    currentSentiment = u8"중립";
}
if (currentSentiment == sFilter) {
    finalFiltered.push_back(item);
}
```

**After (`Filters.cpp` + `SentimentClassifier.cpp`)**

```cpp
// Filters.cpp — filterBySentiment
if (fa::classifySentiment(item.getText()) == sentimentFilter) {
    result.push_back(item);
}

// SentimentClassifier.cpp
std::string classifySentiment(const std::string& text) {
    const PolarityScore score = computeWeightedPolarity(text);
    if (score.positive > score.negative && score.positive > 0.0) {
        return u8"긍정";
    }
    if (score.negative > score.positive && score.negative > 0.0) {
        return u8"부정";
    }
    return u8"중립";
}
```

### 4.2 DEF-02 — 키워드 필터 `main` 스캔

**Before (레거시 `Filters.h` — `main` 스킵)**

```cpp
for (const auto& subEntry : catMap) {
    if (subEntry.first == "main") continue;
    if (containsAny(txt, subEntry.second)) {
        finalFiltered.push_back(item);
        break;
    }
}
```

**After (`Filters.cpp` — 전 서브맵 스캔, `main` 포함)**

```cpp
for (const auto& subEntry : categoryMap) {
    if (fa::containsAny(text, subEntry.second)) {
        result.push_back(item);
        break;
    }
}
```

### 4.3 DEF-03 — download 뷰 모델

**Before (레거시 `main.cpp`)**

```cpp
static std::vector<Feedback> fil_data;

// /filter 성공 시에만
if (!filtered.empty()) {
    fil_data = filtered;
}

// /download
for (const auto& iter : fil_data) {
    csv << iter.getText() << "\n";
}
```

**After (`Session.cpp`)**

```cpp
void Session::refreshAfterAnalyze(const std::vector<Feedback>& feedbacks) {
    currentFeedbacks = feedbacks;
    downloadView_ = currentFeedbacks;
    hasFilterView_ = false;
}

void Session::applyFilterResult(const std::vector<Feedback>& filtered, bool success) {
    if (success && !filtered.empty()) {
        downloadView_ = filtered;
        hasFilterView_ = true;
    }
}

const std::vector<Feedback>& Session::getDownloadView() {
    return downloadView_;
}
```

### 4.4 DEF-04 — CSV `text` 컬럼

**Before (레거시 `main.cpp`)**

```cpp
auto fields = parseCsvLine(line);
if (!fields.empty() && !fields[0].empty()) {
    feedbacks.push_back(Feedback(fields[0]));
}
```

**After (`CsvUploadParser.cpp`)**

```cpp
const auto headerFields = parseLine(line);
int textColumn = 0;
for (size_t i = 0; i < headerFields.size(); ++i) {
    if (headerFields[i] == "text") {
        textColumn = static_cast<int>(i);
        break;
    }
}
// ...
const auto fields = parseLine(line);
if (static_cast<int>(fields.size()) > textColumn &&
    !fields[textColumn].empty()) {
    result.emplace_back(fields[textColumn]);
}
```

### 4.5 God Module — `main.cpp` 축소

**Before (레거시, 요약)**

```cpp
static std::vector<Feedback> fil_data;
static TextAnalyzer textAnalyzer;
static Filters filters;
static FileHandler fileHandler;

static std::string renderPage(...) { /* ~130줄 HTML */ }
static std::vector<std::string> parseCsvLine(...) { ... }

int main() {
    httplib::Server svr;
    svr.Post("/analyze", [&](...) { /* 핸들러 본문 */ });
    svr.Post("/filter", [&](...) { ... });
    svr.Get("/download", [&](...) { ... });
    svr.Post("/upload", [&](...) { ... });
    svr.listen(...);
}
```

**After (`main.cpp` — 23줄 부트스트랩)**

```cpp
int main() {
    Constants::init();

    httplib::Server server;
    TextAnalyzer textAnalyzer;
    Filters filters;
    AppContext context{textAnalyzer, filters};

    HttpRouter::registerRoutes(server, context);

    Logger::logInfo(u8"서버가 http://localhost:8080 에서 시작됩니다.");
    server.listen("0.0.0.0", 8080);

    return 0;
}
```

---

## 5. 효과 및 향후 과제

### 5.1 달성 효과

| 영역 | 효과 |
|------|------|
| **기능 정합** | DEF-01~04 Closed · README·Mom H2/H4/H5/H6 자동 검증 Pass |
| **구조 품질** | God Module 해소 · 감정/키워드 정책 단일화 · `fil_data` 제거 |
| **테스트 자산** | FA-TC + support Domain + GM 5종 · ctest 70/70 회귀 축 |
| **품질 게이트** | lcov Domain 97.3% · Boundary 92.3% · CI 스크립트 자동화 |
| **프로세스** | RED→GREEN→Refactor 순서로 레거시 무수정 red·안전 승격 검증 |
| **기능 확장** | Trend 집계·`saveResult`·가중치 감성(AC-SENT-02) · FA-TC-55~58 |

### 5.2 정량 Before / After

| 지표 | Before (spec/red) | After (2026-05-22) |
|------|-------------------|---------------------|
| `main.cpp` 규모 | ~372줄 | **23줄** |
| DEF-01 대표 `fil(중립)` | 0건 | **1건** (GM-02) |
| `id,text` 업로드 본문 | `1` | **`안녕`** (GM-04) |
| ctest | 의도적 FAIL | **70/70** PASS |
| Domain 커버리지 | — | **97.3%** |

### 5.3 잔여 리스크

| 항목 | 설명 | 권장 |
|------|------|------|
| H3 업로드 후 통계 | `/upload` 직후 `sent`/`kw` 미호출 | upload→analyze 트리거 또는 UI 안내 (미션 3) |
| kw vs fil 스캔 범위 | 집계는 `main`만 · 필터는 전 서브키 | UI에 집계 기준 명시 |
| HTTP 통합 ctest | `feedback_analyzer_integration_tests` NOT_BUILT | CMake에 exe 선행 빌드 의존 |
| AC-SENT-01 vs 02 | feature 후 FA-TC-05/07 기대 변경 | AC 버전을 TC·GM에 태깅 |

### 5.4 향후 과제

| 우선순위 | 과제 | 산출물 |
|----------|------|--------|
| P1 | HTTP 통합 8건 CI 기본 경로 편입 | CMake 타겟·ctest Green |
| P1 | H3 해소 — 업로드 후 분석/통계 | AC·FA-TC-38 Green |
| P2 | `docs/architecture.md` (TODO #11) | 모듈·시퀀스 다이어그램 |
| P2 | Mom Test H1~H6 수동 체크리스트 자동화 | 스모크 스크립트 |
| P3 | 프로덕션 경로 lcov 확대 | `HttpRouter`·`HtmlRenderer` 게이트 정책 정의 |

### 5.5 참조 문서

| 문서 | 경로 |
|------|------|
| 요구사항 분석 | `docs/requirements_analysis.md` |
| 코드 품질 분석 | `docs/code_quality_report.md` |
| 테스트 계획 | `docs/test_plan.md` |
| 리팩토링 계획 | `docs/refactoring_plan.md` |
| 결함 목록 | `docs/defect_list.md` |
| QA 최종 | `docs/qa_final_report.md` |
| 커버리지 | `docs/coverage_report.md` |
| 세션 Report | `Report/00`~`13.*-report-2026-05-22.md` |

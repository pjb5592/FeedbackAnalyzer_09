# Feedback Analyzer
![feedback_analyzer](./feedback_analyzer.png)

고객 피드백 분석 시스템은 자연어 기반 고객 피드백 데이터를 수집, 분류, 시각화하는 기능을 제공하는 C++ (cpp-httplib) 기반 웹 애플리케이션입니다.

## 주요 기능

- 텍스트 피드백 입력 (수동/CSV 업로드)
- 키워드 기반 피드백 분류
- 감정 분석 (긍정/부정/중립)
- 피드백 필터링 및 검색
- 분석 결과 시각화
- 결과 CSV 다운로드

## 요구사항

- C++17 이상 지원 컴파일러 (MSVC, GCC, Clang)
- CMake 3.14 이상

## 설치 방법
저장소 클론
```
git clone [repository-url]
cd feedback_analyzer_cpp
```

## 빌드 방법
```
rmdir /q /s build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/mingw64/bin/g++.exe
cmake --build build
```

## 실행 방법
```
build\feedback_analyzer.exe
```

## 프로젝트 구조

```
feedback_analyzer_cpp/
├── src/cpp/
│   ├── main.cpp              # 부트스트랩 (Constants·Router·listen)
│   ├── HttpRouter.h/cpp      # HTTP 라우트·폼 파싱
│   ├── HtmlRenderer.h/cpp    # HTML 렌더링
│   ├── httplib.h             # cpp-httplib 헤더 라이브러리
│   ├── Feedback.h            # 피드백 데이터 모델
│   ├── TextAnalyzer.h/cpp    # 감정·키워드 분석
│   ├── Filters.h/cpp         # 필터링
│   ├── KeywordMatcher.h      # 키워드 매칭 공통
│   ├── SentimentClassifier.h/cpp  # 감성 분류 공통
│   ├── CsvUploadParser.h/cpp # CSV 업로드 파싱
│   ├── UIComponents.h/cpp    # UI 컴포넌트
│   ├── Session.h/cpp         # 세션·download 뷰
│   ├── Logger.h/cpp          # 로깅
│   └── Constants.h/cpp       # 상수 정의
├── CMakeLists.txt          # CMake 빌드 설정
├── project_purpose.md      # 프로젝트 목적 문서
└── README.md               # 프로젝트 설명
```

## 사용 방법

1. 웹 브라우저에서 `http://localhost:8080` 접속
2. 피드백 텍스트 입력 또는 CSV 파일 업로드
3. 감정/키워드 필터로 결과 필터링
4. 필요시 결과 다운로드

## CSV 파일 형식

입력 CSV 파일은 다음과 같은 형식이어야 합니다:
- 필수 컬럼: `text`
- 텍스트 컬럼에 피드백 내용 포함

---

## TDD/리팩토링 진행 TODO

> 브랜치 순서: `spec` → `red` → `green` → `refactoring` → `feature/newFeature`  
> 규칙 상세: 프로젝트 루트 [`.cursorrules`](./.cursorrules) · 프롬프트: [`prompt초안.md`](./prompt초안.md)

| # | 단계 | 브랜치 | 산출물 | 진행 |
|---|------|--------|--------|------|
| 0 | 프로젝트 규칙 | spec | `.cursorrules` | [x] |
| 1 | 요구사항 분석 | spec | `docs/requirements_analysis.md` | [x] |
| 2 | 코드 품질 분석 | spec | `docs/code_quality_report.md` | [x] |
| 3 | 테스트 계획 (Test Plan) | spec | `docs/test_plan.md` | [x] |
| 4-A | RED — 실패 테스트 | red | `tests/*.cpp`, ctest 의도적 FAIL | [x] |
| 4-B | GREEN — FA-TC 통과 | green | `tests/support/`, FA-TC 전건 Green | [x] |
| 4-C | 커버리지 게이트 | green | `docs/coverage_report.md`, Domain≥90% Boundary≥85% | [x] |
| 5 | 리팩토링 계획 | spec / refactoring | `docs/refactoring_plan.md` | [x] (2026-05-22, `refactoring`, Phase 0~4 · 12 Step) |
| 6 | 리팩토링 실행 | refactoring | Phase·Step별 1 Commit, ctest Green | [x] (2026-05-22, Phase 0~4 · 12 Commit) |
| 7 | 결함 분석·문서화 | green / refactoring | `docs/defect_list.md` | [ ] (다음) |
| 8 | Golden Master (4-D) | green | `tests/golden/*.approved.txt`, `docs/golden_master.md` (4-B·4-C 후) | [x] |
| 9 | 기능 개선 | feature/newFeature | Trend·File DB 등 (미션 6~7) | [ ] |
| 10 | 결함 관리 프로세스 | spec | `docs/defect_report.md` | [ ] |
| 11 | 설계 다이어그램 (선택) | spec | `docs/architecture.md` | [ ] |
| 12 | QA 종합 검토 | refactoring / main | `docs/qa_final_report.md` | [ ] |

**green 완료 조건 (refactoring 진입 전, 순서 고정)**

- [x] FA-TC 전건 `ctest` Green (**67/67**, 2026-05-22)
- [x] `scripts/run_coverage_gate.ps1` PASS (Domain **97.6%** · Boundary **85.0%** · Overall **98.1%**)
- [x] Golden Master `ctest` Green (**GM-01~04**, `GoldenMaster` 집계, FA-TC-53)

**refactoring TODO** (`refactoring` 브랜치 · [`docs/refactoring_plan.md`](./docs/refactoring_plan.md) · **1 Step = 1 Commit**)

> 매 Step 후: `ctest` 67/67 + `GoldenMaster` + `run_coverage_gate.ps1` Green 유지. `src/cpp/`만 수정.

| Phase | DEF | Step | 내용 | 진행 |
|-------|-----|------|------|------|
| **0** | DEF-01 | 0.1 | `Filters` 감정 분기 = `Constants` + `sent` 규칙 | [x] |
| **0** | DEF-01 | 0.2 | `S_KEYWORDS`·`initFilterKeywords` 제거 | [x] |
| **1** | DEF-02 | 1.1 | `fil` 카테고리 스캔에 `main` 포함 | [x] |
| **2** | DEF-03 | 2.1 | Session download 뷰 API | [x] |
| **2** | DEF-03/04 | 2.2 | `fil_data` 제거 · analyze/filter/download 연동 | [x] |
| **2** | DEF-04 | 2.3 | `CsvUploadParser` 승격 · `/upload` `text` 컬럼 | [x] |
| **3** | 중복 | 3.1 | `KeywordMatcher` 공통화 | [x] |
| **3** | 네이밍 | 3.2 | `fil`→`filterFeedbacks` 등 API rename | [x] |
| **3** | 전역 | 3.3 | `globalSent`/`globalKw`·`cout`·Session dead code 정리 | [x] |
| **4** | God Module | 4.1 | `HtmlRenderer` 분리 | [x] |
| **4** | God Module | 4.2 | `HttpRouter` 분리 | [x] |
| **4** | God Module | 4.3 | `main.cpp` 부트스트랩만 | [x] |
| **5** | DRY·헤더 | 5.1 | `Filters`·`TextAnalyzer` `.cpp` 분리 | [x] |
| **5** | DRY | 5.2 | `SentimentClassifier` 추출 | [x] |
| **5** | Shotgun | 5.3 | 카테고리 `Constants` 단일 소스 | [x] |
| **5** | Data | 5.4 | `Constants::init` 중복 제거 | [x] |
| **6** | Session | 6.1 | 피드백 접근 캡슐화 | [x] |
| **7** | Infra | 7.1 | `Logger` cpp 분리 | [x] |
| **7** | View | 7.2 | `HtmlRenderer::PageModel` | [x] |
| **7** | Router | 7.3 | `HttpRouter` 응답 DRY | [x] |

**Phase 완료 체크 (GM·FA-TC)**

- [x] Phase 0 — FA-TC-17/29/32 · **GM-02**
- [x] Phase 1 — FA-TC-16/24/30
- [x] Phase 2 — FA-TC-33~34, 39~41 · **GM-03**, **GM-04**
- [x] Phase 3~4 — GM-01~04 전건 · `feedback_analyzer.exe` smoke (FA-TC-44~52)

**목표 계약:** AC-SENT-01 · AC-KW-01/02 · download 뷰 · CSV `text` (→ `docs/defect_list.md` Phase 완료 후 기록)

**검증 명령**

```bash
cmake --build build
ctest --test-dir build --output-on-failure
ctest --test-dir build -R "^GoldenMaster$"
.\scripts\run_coverage_gate.ps1 -BuildDir build-cov
```

**Golden Master 재생성 (게이트 통과 후):**

```powershell
.\scripts\generate_golden_master.ps1 -Force
```

**테스트 타겟:** `feedback_analyzer_tests` (Catch2) · `golden_master_gen` (스냅샷 생성)

**커버리지 게이트 (4-C):**

```powershell
.\scripts\run_coverage_gate.ps1 -BuildDir build-cov
```

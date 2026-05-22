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
│   ├── main.cpp           # HTTP 서버 및 라우팅 (cpp-httplib 기반)
│   ├── httplib.h           # cpp-httplib 헤더 라이브러리
│   ├── Feedback.h          # 피드백 데이터 모델
│   ├── TextAnalyzer.h/cpp  # 텍스트 분석 로직
│   ├── Filters.h/cpp       # 필터링
│   ├── UIComponents.h/cpp  # UI 컴포넌트
│   ├── Session.h/cpp       # 상태 관리
│   ├── Logger.h/cpp        # 로깅
│   ├── Constants.h/cpp     # 상수 정의
│   └── FileHandler.h       # 파일 처리
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
| 5 | 리팩토링 계획 | spec / refactoring | `docs/refactoring_plan.md` | [ ] |
| 6 | 리팩토링 실행 | refactoring | Phase·Step별 1 Commit, ctest Green | [ ] |
| 7 | 결함 분석·문서화 | green / refactoring | `docs/defect_list.md` | [ ] |
| 8 | Golden Master (4-D) | green | `tests/golden/*.approved.txt`, `docs/golden_master.md` (4-B·4-C 후) | [x] |
| 9 | 기능 개선 | feature/newFeature | Trend·File DB 등 (미션 6~7) | [ ] |
| 10 | 결함 관리 프로세스 | spec | `docs/defect_report.md` | [ ] |
| 11 | 설계 다이어그램 (선택) | spec | `docs/architecture.md` | [ ] |
| 12 | QA 종합 검토 | refactoring / main | `docs/qa_final_report.md` | [ ] |

**green 완료 조건 (refactoring 진입 전, 순서 고정)**

- [x] FA-TC 전건 `ctest` Green (**67/67**, 2026-05-22)
- [x] `scripts/run_coverage_gate.ps1` PASS (Domain **97.6%** · Boundary **85.0%** · Overall **98.1%**)
- [x] Golden Master `ctest` Green (**GM-01~04**, `GoldenMaster` 집계, FA-TC-53)

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

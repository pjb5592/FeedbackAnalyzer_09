# 코드 품질 정적 분석 보고서

| 항목 | 내용 |
|------|------|
| 프로젝트 | Feedback Analyzer (C++17 / cpp-httplib) |
| 분석 일자 | 2026-05-22 |
| 분석 범위 | `main.cpp`, `TextAnalyzer.h`, `Filters.h/cpp`, `Constants.h/cpp`, `Session.h/cpp`, `Feedback.h`, `FileHandler.h`, `README.md` |
| 분석 관점 | SOLID 원칙 위반, Code Smell, 의도적 레거시 결함(DEF-01~04) |
| 성격 | **학습·TDD용 의도적 스멜** — 프로덕션 품질 기준 미충족이 설계상 전제 |

---

## 1. 요약

현재 코드베이스는 **단일 실행 파일(`main.cpp`)에 HTTP·HTML·CSV·라우팅·상태가 집중**되어 있고, 도메인 로직(`TextAnalyzer`, `Filters`)은 **별도 키워드 사전·`containsAny` 중복**으로 분리되어 있다. 그 결과 **집계(`sent`/`kw`)와 필터(`fil`)의 계약 불일치(DEF-01, DEF-02)** 가 구조적으로 고착되어 있으며, **전역·정적 상태(`Session`, `globalSent/Kw`, `fil_data`)** 가 테스트·동시성·재현성을 저해한다.

리팩토링은 **P0: 도메인 계약·결함(DEF)** → **P1: 상태·중복 제거** → **P2: 모듈 경계(God Module, Shotgun Surgery)** → **P3: 네이밍·죽은 코드** 순이 합리적이다. (`.cursorrules` Phase 0~4 및 green 게이트 이후 실행 전제)

---

## 2. Code Smell 상세

### 2.1 God Module — `main.cpp`

| 지표 | 관측 |
|------|------|
| 라인 수 | 약 372줄 (도메인·인프라·프레젠테이션 혼재) |
| 책임 | HTTP 서버, 라우트 5개, HTML 렌더(~130줄), URL/폼 파싱, CSV 파싱, 타임스탬프, 예외 처리, 앱 부트스트랩 |

**근거:** `main.cpp` 한 파일에 다음이 모두 존재한다.

- 정적 전역: `fil_data`, `textAnalyzer`, `filters`, `fileHandler` (16~19행)
- 유틸: `urlDecode`, `parseForm`, `escapeHtml`, `parseCsvLine` (22~231행)
- UI: `renderPage` — 인라인 CSS·폼·통계 HTML (80~211행)
- 오케스트레이션: `main()` 내 `httplib::Server` 라우트 등록 및 핸들러 본문 (233~371행)

```16:19:src/cpp/main.cpp
static std::vector<Feedback> fil_data;
static TextAnalyzer textAnalyzer;
static Filters filters;
static FileHandler fileHandler;
```

```80:86:src/cpp/main.cpp
static std::string renderPage(const std::string& success,
                               const std::string& warning,
                               const std::string& error,
                               const std::map<std::string, int>& sentimentResults,
                               const std::map<std::string, int>& keywordResults,
                               const std::vector<Feedback>& feedbacks) {
```

**SOLID:** **SRP** 위반(단일 변경 사유가 UI/HTTP/파일/분석으로 분산), **OCP** 취약(새 엔드포인트·뷰마다 `main.cpp` 수정).

**영향:** UI 문구 변경도 컴파일 단위 전체 재빌드, 단위 테스트 불가(정적 함수·전역 결합).

---

### 2.2 Duplicate Code — `containsAny`

동일한 부분 문자열 매칭 헬퍼가 **비공개 static으로 이중 정의**된다.

| 위치 | 파일 | 행 |
|------|------|-----|
| A | `TextAnalyzer.h` | 13~18 |
| B | `Filters.h` | 13~18 |

```13:18:src/cpp/TextAnalyzer.h
    static bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
        for (const auto& kw : keywords) {
            if (text.find(kw) != std::string::npos) return true;
        }
        return false;
    }
```

```13:18:src/cpp/Filters.h
    static bool containsAny(const std::string& text, const std::vector<std::string>& keywords) {
        for (const auto& kw : keywords) {
            if (text.find(kw) != std::string::npos) return true;
        }
        return false;
    }
```

**위험:** 알고리즘 변경(대소문자, 단어 경계, 정규화) 시 **두 곳 동시 수정** 필요 → Shotgun Surgery와 결합.

**권장 방향:** `namespace detail` 또는 `KeywordMatcher::containsAny` 단일 구현 (P1).

---

### 2.3 Global State (전역·정적 상태)

| 심볼 | 위치 | 용도 | 문제 |
|------|------|------|------|
| `fil_data` | `main.cpp:16` | 마지막 필터 결과, `/download` 소스 | Session과 이중 저장, 필터 전 다운로드 오류(DEF-03) |
| `TextAnalyzer::globalSent`, `globalKw` | `TextAnalyzer.h:10~11`, `.cpp:3~4` | 집계 부작용 | 반환값과 중복, 테스트 격리 불가 |
| `Session::currentFeedbacks` | `Session.h:9`, `.cpp:3` | 피드백 목록 | `getOldDataFromSession`가 key 무시 |
| `Filters::S_KEYWORDS` | `Filters.h:11`, `.cpp:3` | 필터 전용 감정 사전 | `Constants::SENTIMENT_KEYWORDS`와 분리(DEF-01) |
| `Constants::SENTIMENT_KEYWORDS`, `CATEGORY_KEYWORDS` | `Constants.h:8~9` | 집계·일부 필터 | mutable static map |
| `fileHandler` | `main.cpp:19` | — | **미호출** (Lava Flow 연계) |

```38:39:src/cpp/TextAnalyzer.h
        globalSent = res;
        return res;
```

```18:20:src/cpp/Session.h
    static std::vector<Feedback>& getOldDataFromSession(const std::string& key) {
        return currentFeedbacks;
    }
```

**SOLID:** **DIP** 부재 — 핸들러가 구체적 static에 직접 의존. **테스트:** Catch2에서 순서 의존·오염 가능.

---

### 2.4 Feature Envy — `Filters` ↔ `Constants`

`Filters`는 자체 `S_KEYWORDS`를 갖지만, 키워드 필터 단계에서는 **`Constants::CATEGORY_KEYWORDS` 내부 구조를 직접 순회**한다.

```53:61:src/cpp/Filters.h
                if (Constants::CATEGORY_KEYWORDS.count(kFilter)) {
                    const auto& catMap = Constants::CATEGORY_KEYWORDS[kFilter];
                    for (const auto& subEntry : catMap) {
                        if (subEntry.first == "main") continue;
                        if (containsAny(txt, subEntry.second)) {
                            finalFiltered.push_back(item);
                            break;
```

반면 `TextAnalyzer::kw`는 **`"main"` 키만** 사용한다.

```50:57:src/cpp/TextAnalyzer.h
            for (const auto& entry : Constants::CATEGORY_KEYWORDS) {
                const std::string& cat = entry.first;
                if (entry.second.count("main")) {
                    const auto& kws = entry.second.at("main");
                    if (containsAny(txt, kws)) {
                        res2[cat]++;
```

**Feature Envy:** `Filters`가 `Constants`의 map-nested-map 레이아웃(`main` vs 서브키)에 대한 **지식이 과다** — 카테고리 정책은 `Constants` 또는 전용 `CategoryClassifier` 소유가 타당.

**SOLID:** **SRP** — Constants는 데이터 홀더인데 분류 정책이 Filters/TextAnalyzer에 분산.

---

### 2.5 Shotgun Surgery (산탄총 수술)

**한 가지 비즈니스 규칙 변경**이 여러 파일을 동시에 건드린다.

| 변경 시나리오 | 수정 필요 파일/위치 |
|---------------|---------------------|
| 감정 키워드 추가·수정 | `Constants.cpp` (`SENTIMENT_KEYWORDS`), `Filters.cpp` (`S_KEYWORDS`) — **이중 유지** |
| 중립 판정 규칙 통일 | `TextAnalyzer.h` (`sent`), `Filters.h` (`fil` 감정 분기) |
| 카테고리 `main` 정책 | `TextAnalyzer.h` (`kw`), `Filters.h` (`main` skip) |
| 다운로드 대상 | `main.cpp` (`fil_data` 할당 332행, `/download` 361행), 필요 시 `Session` |
| CSV 컬럼 규칙 | `main.cpp` `parseCsvLine` + upload 핸들러 (303~306행) |

**근거:** DEF-01·DEF-02는 단일 버그가 아니라 **키워드 소스·스캔 정책이 파일 단위로 갈라진 구조적 결과**이다.

---

### 2.6 Lava Flow — `FileHandler`

README·프로젝트 구조에는 “파일 처리”로 기술되나, **런타임 경로에서 호출되지 않는다.**

```7:18:src/cpp/FileHandler.h
class FileHandler {
public:
    void saveResult(const std::vector<Feedback>& data) {
        std::cout << "saveResult" << data.size() << std::endl;
        for (const auto& iter : data)
            std::cout << iter.getText() << std::endl;
    }

    void save(const std::vector<Feedback>& data) {
        saveResult(data);
    }
};
```

```19:19:src/cpp/main.cpp
static FileHandler fileHandler;
```

실제 CSV 저장·다운로드는 **`main.cpp`의 `/download` 핸들러가 직접 `ostringstream`으로 생성**한다 (356~366행). `FileHandler`는 include·static 인스턴스만으로 **빌드 의존성만 증가**시킨다.

**추가 Lava Flow 후보:** `Session::internalData`, `filterOptions` — 선언만 있고 읽기/쓰기 경로 없음 (`Session.cpp:4~5`).

---

## 3. SOLID 원칙 위반 요약

| 원칙 | 위반 요약 | 대표 위치 |
|------|-----------|-----------|
| **S**RP | `main.cpp` = HTTP + HTML + CSV + wiring | `renderPage`, 라우트 람다 |
| **O**CP | 새 분석 규칙 시 Analyzer·Filters·Constants 동시 수정 | Shotgun Surgery 표 |
| **L**SP | 해당 낮음 (상속 계층 거의 없음) | — |
| **I**SP | 거대한 `renderPage` 파라미터 6개 | `main.cpp:81~86` |
| **D**IP | 핸들러 → static Session/global 직접 참조 | `Session::getCurrentFeedbacks()` |

---

## 4. 의도적 결함 DEF-01~04 (근거 코드)

### DEF-01 — 감정: `sent` vs `fil` 규칙 불일치

| 항목 | `TextAnalyzer::sent` | `Filters::fil` (감정) |
|------|----------------------|------------------------|
| 키워드 | `Constants::SENTIMENT_KEYWORDS` | `Filters::S_KEYWORDS` (`Filters.cpp` 5~20행) |
| 중립 | 긍정·부정 아니면 **무조건 중립** | `S_KEYWORDS["중립"]` **매칭 시에만** 중립 |

**집계 (`sent`):**

```27:35:src/cpp/TextAnalyzer.h
        for (const auto& f : feedbacks) {
            std::string txt = f.getText();
            std::string s = u8"중립";
            if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"긍정"])) {
                s = u8"긍정";
            } else if (containsAny(txt, Constants::SENTIMENT_KEYWORDS[u8"부정"])) {
                s = u8"부정";
            }
            res[s]++;
```

**필터 (`fil`):**

```31:42:src/cpp/Filters.h
                std::string currentSentiment = u8"중립";

                if (containsAny(txt, S_KEYWORDS[u8"긍정"])) {
                    currentSentiment = u8"긍정";
                } else if (containsAny(txt, S_KEYWORDS[u8"부정"])) {
                    currentSentiment = u8"부정";
                } else if (containsAny(txt, S_KEYWORDS[u8"중립"])) {
                    currentSentiment = u8"중립";
                }

                if (currentSentiment == sFilter) {
```

**재현 예:** `"보통 배송은 괜찮아요"` — `sent`는 중립 키워드 사전에 `보통`·`괜찮` 혼재 가능하나, **집계는 부정 미매칭 시 중립++**; 필터 `중립` 선택 시 `S_KEYWORDS["중립"]`만 검사 → **0건 가능**.

**교차 키워드:** `괜찮`이 `Filters.cpp` 긍정(8행)과 중립(18행) 양쪽에 존재 → 모듈별 우선순위 차이 증폭.

**Mom Test:** H1, H4 Fail 신호 (`.cursorrules` §7).

---

### DEF-02 — 키워드: `kw`는 `"main"` 사용, `fil`은 `"main"` 스킵

**집계 (`kw`) — main만:**

```50:56:src/cpp/TextAnalyzer.h
                if (entry.second.count("main")) {
                    const auto& kws = entry.second.at("main");
                    if (containsAny(txt, kws)) {
                        res2[cat]++;
                    }
                }
```

**필터 — main 제외, 서브키만:**

```55:60:src/cpp/Filters.h
                    for (const auto& subEntry : catMap) {
                        if (subEntry.first == "main") continue;
                        if (containsAny(txt, subEntry.second)) {
                            finalFiltered.push_back(item);
                            break;
```

**재현 예:** `"배송"`만 포함 — `kw`는 `CATEGORY_KEYWORDS["배송"]["main"]` 매칭 → +1; `fil(배송)`은 서브키(`time`,`type`…)만 검사 → **0건 가능**.

**Mom Test:** H2 Fail (`.cursorrules` §7).

---

### DEF-03 — `/download`가 `fil_data`만 사용

**필터 성공 시에만 갱신:**

```330:332:src/cpp/main.cpp
                auto filtered = filters.fil(feedbacks, sentiment, keyword);
                if (!filtered.empty()) {
                    fil_data = filtered;
```

**다운로드는 Session이 아닌 `fil_data`:**

```356:363:src/cpp/main.cpp
    svr.Get("/download", [](const httplib::Request&, httplib::Response& res) {
        std::ostringstream csv;
        csv << "\xEF\xBB\xBF";
        csv << "text\n";
        for (const auto& iter : fil_data) {
            csv << iter.getText() << "\n";
```

| 시나리오 | 결과 |
|----------|------|
| 필터 전 `/download` | `fil_data` 빈 벡터 → **빈 CSV** |
| 이전 필터 후 새 입력만 | **스테일 필터 결과** 다운로드 가능 |
| `/analyze` 직후 | `fil_data` 미갱신 — 화면 통계와 다운로드 불일치 |

**Mom Test:** H5 Fail.

---

### DEF-04 — CSV `text` 컬럼 미인식

README는 **필수 컬럼 `text`** 를 명시한다 (`README.md` 68~70행).

구현은 **첫 줄 스킵 + `fields[0]`만** 피드백으로 사용한다.

```299:306:src/cpp/main.cpp
                    while (std::getline(stream, line)) {
                        if (!line.empty() && line.back() == '\r') line.pop_back();
                        if (firstLine) { firstLine = false; continue; }
                        if (line.empty()) continue;
                        auto fields = parseCsvLine(line);
                        if (!fields.empty() && !fields[0].empty()) {
                            feedbacks.push_back(Feedback(fields[0]));
```

| CSV 예 | 기대 | 실제 |
|--------|------|------|
| `text\n안녕` | `안녕` | col0=`안녕` → OK |
| `id,text\n1,안녕` | `안녕` | col0=`1` → **오적재** |
| 헤더명 `text` 검증 | 있음 | **없음** |

**부가 갭:** `/upload` 후 `sent`/`kw` 미호출 (311~312행은 건수 메시지만) — README “업로드 후 분석” 시나리오와 UX 불일치.

**Mom Test:** H3, H6 Fail.

---

## 5. 모듈별 품질 스냅샷

| 모듈 | 역할 | 품질 이슈 |
|------|------|-----------|
| `Feedback.h` | 값 객체 | 양호 — 단일 `text`, 이동 생성 |
| `Constants` | 키워드 데이터 | 데이터+초기화만 — **정책은 소비자에 분산** |
| `TextAnalyzer` | 집계 | global 부작용, `containsAny` 중복, DEF-01/02 소비 측 |
| `Filters` | 필터 | `S_KEYWORDS` 이중화, `cout` 부작용(68~70행), DEF-01/02 제공 측 |
| `Session` | 상태 | key 무시 API, 미사용 멤버 |
| `main.cpp` | 앱 전체 | God Module, DEF-03/04, 미사용 `fileHandler` |
| `FileHandler` | (명목상) 파일 | Lava Flow — 미연결 |

---

## 6. 리팩토링 우선순위 (P0~P3)

> **전제:** FA-TC RED → GREEN → 커버리지 게이트 완료 후 단계적 리팩토링 (`.cursorrules` §9~10). 각 Step 후 `ctest` Green 유지.

### P0 — 도메인 계약·의도적 결함 (최우선)

| # | 작업 | 근거 | 변경 후보 | 연계 |
|---|------|------|-----------|------|
| P0-1 | **감정 단일 소스** — `S_KEYWORDS` 제거, `Constants::SENTIMENT_KEYWORDS` + **동일 판정 순서** | DEF-01, H4, Shotgun(감정) | `Filters.cpp`, `Filters.h`, `TextAnalyzer.h` | Phase 0 |
| P0-2 | **키워드 `main` 정책 통일** — 집계·필터 동일 키 집합 | DEF-02, H2 | `TextAnalyzer.h`, `Filters.h` | Phase 1 |
| P0-3 | **다운로드 소스 일원화** — `fil_data` 제거, Session 또는 “현재 뷰 모델” | DEF-03, H5 | `main.cpp` filter/download 핸들러 | Phase 2 |
| P0-4 | **CSV `text` 컬럼 파싱** — 헤더 인식, 컬럼 인덱스 해석 | DEF-04, H6 | `main.cpp` upload + `parseCsvLine` | Phase 2 |

**왜 P0인가:** 사용자 가설(H1~H6)과 FA-TC P0가 **동작 불일치**에 묶여 있으며, 구조 개선만으로는 테스트가 의도적 FAIL/관측 계약을 잃을 수 있음. **계약을 먼저 고정**해야 리팩토링이 회귀 검출 가능.

---

### P1 — 중복·전역 상태 (유지보수·테스트성)

| # | 작업 | 근거 |
|---|------|------|
| P1-1 | `containsAny` **단일 구현** 추출 | Duplicate Code, Shotgun |
| P1-2 | `globalSent` / `globalKw` **제거** — 순수 함수 반환만 | Global State, 테스트 격리 |
| P1-3 | `Session` API 정리 — `getOldDataFromSession(key)` 시맨틱 수정 또는 삭제 | Misleading API |
| P1-4 | `Filters::fil` 내 `std::cout` 제거 → `Logger` | 부작용, SRP |

**왜 P1인가:** P0 계약 수정 후에도 전역·중복이 남으면 **다음 규칙 변경이 다시 Shotgun**이 된다.

---

### P2 — 구조 분리 (God Module·Envy)

| # | 작업 | 근거 |
|---|------|------|
| P2-1 | `HtmlRenderer` (또는 템플릿 빌더) — `renderPage`·`escapeHtml` 분리 | God Module, SRP |
| P2-2 | `HttpRouter` / 핸들러 클래스 — 라우트·요청 파싱 분리 | God Module, DIP |
| P2-3 | `CategoryClassifier` / `SentimentClassifier` — Constants 순회 로직 캡슐화 | Feature Envy |
| P2-4 | `CsvParser` — 업로드·다운로드 공통 | DEF-04 구현체, SRP |

**왜 P2인가:** P0·P1 없이 분리만 하면 **잘못된 계약이 모듈 경계로만 이동**한다.

---

### P3 — 정리·가독성 (낮은 리스크)

| # | 작업 | 근거 |
|---|------|------|
| P3-1 | `FileHandler` **삭제 또는 실제 구현** (`/download` 위임) | Lava Flow |
| P3-2 | 네이밍: `fil`→`filterFeedbacks`, `sent`→`analyzeSentiment`, `kw`→`countKeywords` | 가독성, 미션 4 |
| P3-3 | `Constants.cpp` 중복 키워드 라인 정리 (7~12, 15~20행 등) | 데이터 품질 |
| P3-4 | `Session::internalData`, `filterOptions` 미사용 제거 | Dead code |

**왜 P3인가:** 동작·구조 안정 후 **이름·죽은 코드**는 리스크 대비 효용이 크다.

---

## 7. 우선순위 ↔ 문서·브랜치 매핑

| 우선순위 | `.cursorrules` Phase | 산출물 | README TODO |
|----------|-------------------|--------|-------------|
| P0 | 0~2 | `docs/refactoring_plan.md`, `docs/defect_list.md` | #5 리팩토링 계획 → #6 실행 |
| P1 | 3 (일부) | 동일 | 미션 4 전역 상태 |
| P2 | 4 | `HtmlRenderer`, `Router` | 미션 5 긴 함수 |
| P3 | 3~4 잔여 | — | 미션 4 네이밍 |

---

## 8. 모던 C++ 관점 권고 (요약)

| 항목 | 현재 | 권장 |
|------|------|------|
| 상태 | mutable static | 요청 스코프 `AppContext` 또는 DI |
| 문자열 | UTF-8 리터럴 `u8""` 혼용 | 일관된 UTF-8 처리 문서화 |
| API | 축약명 `fil`, `sent`, `kw` | 의도 드러나는 full name |
| HTML | raw string R"(...)" 130줄 | 템플릿 파일 또는 빌더 |
| 예외 | 핸들러별 try/catch 중복 | 공통 핸들러 래퍼 |
| C++17 | 사용 중 | `std::optional`, `string_view`(키워드 매칭) 점진 도입 검토 |

---

## 9. 결론

의도적 레거시는 **God Module·이중 키워드 사전·이중 `containsAny`·다중 전역 상태**가 맞물려 DEF-01~04를 **구조적으로 재현 가능**하게 만든다. 리팩토링은 **P0에서 집계·필터·CSV·다운로드 계약을 단일화**한 뒤, P1에서 중복·전역을 제거하고, P2에서 `main.cpp`를 분리하는 순서가 `.cursorrules` Phase 0~4 및 TDD green 게이트와 정합한다.

**다음 단계 (spec 브랜치):** `docs/test_plan.md` — DEF-01~04·경계값을 FA-TC-ID로 고정 → RED 테스트 작성.

---

## 부록 A — 분석 대상 파일 목록

- `src/cpp/main.cpp`
- `src/cpp/TextAnalyzer.h`, `TextAnalyzer.cpp`
- `src/cpp/Filters.h`, `Filters.cpp`
- `src/cpp/Constants.h`, `Constants.cpp`
- `src/cpp/Session.h`, `Session.cpp`
- `src/cpp/Feedback.h`
- `src/cpp/FileHandler.h`
- `README.md`, `.cursorrules`, `docs/requirements_analysis.md`

## 부록 B — DEF ↔ FA-TC (참고)

| DEF | 대표 TC (requirements_analysis §7) | Mom Test |
|-----|--------------------------------------|----------|
| DEF-01 | FA-006, FA-017, FA-029 | H1, H4 |
| DEF-02 | FA-016, FA-024, FA-030 | H2 |
| DEF-03 | FA-039~FA-042 | H5 |
| DEF-04 | FA-033~FA-034 | H3, H6 |

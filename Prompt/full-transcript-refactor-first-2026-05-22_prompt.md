# Full Transcript — refactor-first workflow

**프로젝트:** Feedback Analyzer_09  
**갱신:** 2026-05-22  
**브랜치:** `spec` (Step 00 완료)

---

## 세션 요약 (누적)

| Step | slug | 산출 | 상태 |
|------|------|------|------|
| **00** | **spec-cursorrules** | `.cursorrules`, README TODO, Report/Prompt | ✅ |

---

## Step 00 상세 (2026-05-22)

**User:** `@README.md` — [P] 레거시 C++ QA/리팩토링 시니어 — `.cursorrules` 작성 (브랜치 전략, DEF-01~04, Mom Test H1~H6, 미션 1~7, green 게이트, README TODO).

**Assistant:**
- `.cursorrules` 신규 (한글, 14개 섹션)
- `README.md` TDD/리팩토링 진행 TODO 표, 단계 0 `[x]`
- `src/cpp/` 수정 없음

**검증:**
- `cmake --build build` → ✅ `feedback_analyzer` 빌드
- `ctest` → ⏭️ 테스트 타겟 없음 (Catch2 미도입)

**User:** [P] 문서·배포 — Step 00 Report/Prompt/full-transcript·git commit/push.

**Assistant:** Report 00 · Transcript 00 · full-transcript 갱신 · commit·push.

**다음:** `spec` — Step 01 `docs/requirements_analysis.md`

---

## 브랜치·의존 로드맵 (참고)

```
main → spec → red → green → refactoring → feature/newFeature → main
```

| # | 브랜치 | 핵심 |
|---|--------|------|
| 0 | spec | `.cursorrules` ✅ |
| 1~3 | spec | requirements · code_quality · test_plan |
| 4-A | red | FA-TC RED |
| 4-B~C, 8 | green | GREEN → 커버리지 → GM |
| 5~6 | refactoring | refactoring_plan Step |
| 9 | feature | 미션 6~7 |

---

*이 파일은 단계 완료 시마다 Step 표와 상세 섹션을追加한다.*

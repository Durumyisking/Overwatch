# Overwatch Production Reference

작성일: 2026-04-19  
기준: 공식 Overwatch 사이트와 Blizzard News 공개 자료 스냅샷

## 결론
우리 프로젝트는 오버워치의 `전체 콘텐츠 양`을 따라가면 안 된다.  
대신 아래 4가지만 먼저 가져와야 한다.

- `5v5 역할 기반 히어로 슈터 구조`
- `서버 authoritative 전투와 명확한 상태 owner`
- `역할과 영웅 개성이 분명한 능력 설계`
- `목표가 읽히는 맵과 HUD`

현재 레포의 학습 목표가 `Dedicated Server + GAS + CommonUI + MVVM`인 만큼, 오버워치의 핵심에서 필요한 것은 `많은 영웅 수`가 아니라 `영웅 확장이 쉬운 구조`다.

---

## 공식 자료에서 확인한 현재 핵심 구조

### 1. 현재 오버워치 PvP의 기본 축
- 공식 홈페이지는 오버워치를 `free-to-play`, `team-based action`, `5v5 battlefield brawl`로 설명한다.
- 공식 PvP 변경 글은 표준 PvP 구성을 `1 Tank, 2 Damage, 2 Support`로 명시한다.
- 공식 영웅 페이지는 역할을 `Tank / Damage / Support`로 구분한다.

### 2. 역할 정체성
- Tank는 피해를 버티고 좁은 길목과 적 집결 지점을 돌파하는 전방 역할이다.
- Damage는 다양한 무기와 능력으로 적을 찾아 처치하는 역할이다.
- Support는 치유, 보호, 강화, 방해로 팀 생존을 책임지는 역할이다.

### 3. 현재 모드 축
- 기본 PvP는 5v5 표준 모드 위에 운영된다.
- Push는 중앙 로봇을 차지하고 상대 진영 쪽으로 밀어붙이는 대칭 구조 모드다.
- Flashpoint는 맵의 여러 거점을 순차적으로 두고 먼저 3개를 차지하는 모드다.
- Stadium은 2025년 4월부터 별도 축으로 운영되는 `best-of-7`, `2팀 5명`, `라운드별 업그레이드 구매`, `1-2-2 고정`, `3인칭 지원` 모드다.

### 4. 라이브 서비스 확장 방향
- 2025년 Spotlight 기준으로 코어 게임플레이에는 `Perks`가 들어갔다.
- 같은 시기 Stadium 전용 빌드, 아이템, 능력 개조, 전용 UI가 추가되었다.
- 공식 영웅 페이지는 `Quick Play`와 `Stadium` 필터를 따로 제공한다.

정리하면, 현재 오버워치는 `단순 FPS`가 아니라 `역할 기반 코어 PvP + 영웅별 능력 차이 + 모드별 별도 규칙층 + 라이브 업데이트용 확장 구조`를 가진 게임이다.

---

## 우리 프로젝트에 실제로 필요한 정보만 추린 판단

### 반드시 가져와야 하는 것

#### A. 5v5 역할 구조
- 장기적으로는 `1 Tank, 2 Damage, 2 Support`를 기준 구조로 잡는 편이 맞다.
- 다만 현재 마일스톤은 2인 전용 서버 검증이므로, 당장은 `최종 팀 크기`보다 `역할과 상태 owner 분리`가 더 중요하다.
- 즉 지금은 `2인 슬라이스`로 검증하되, 자료구조와 팀 규칙은 나중에 5v5로 확장 가능하게 설계해야 한다.

#### B. 영웅 중심 전투 리듬
- 오버워치의 핵심은 조작 자체보다 `영웅별 차별화된 전투 리듬`이다.
- 최소 단위는 보통 다음 조합으로 생각하는 것이 안전하다.
- `기본 공격`
- `이동기 또는 생존기 1개`
- `쿨다운 스킬 1~2개`
- `궁극기 1개`
- 이 구조는 GAS와 잘 맞기 때문에, 우리 프로젝트도 초반부터 `HeroDefinition + AbilitySet + InputConfig` 방향으로 준비하는 것이 좋다.

#### C. 서버 authoritative 구조
- 오버워치식 게임은 피격, 처치, 리스폰, 점수, 점령 진행이 모두 멀티플레이 기준으로 일관되어야 한다.
- 따라서 우리 레포 문서에 이미 적어둔 방향대로 다음 owner를 유지하는 것이 맞다.
- `GameMode`: 서버 규칙과 리스폰 orchestration
- `GameState`: 매치 공용 복제 상태
- `PlayerState`: 플레이어 지속 상태와 향후 ASC owner 후보
- `Character`: 현재 아바타 상태와 전투 실행
- `PlayerController`: 로컬 입력 라우팅과 서버 RPC 시작점

#### D. 목표가 읽히는 맵
- 공식 맵 디자인 글은 `아트와 미관이 게임플레이 복잡도와 깊이를 지원해야 한다`고 설명한다.
- 5v5 전환 후 기존 맵에는 `커버 추가`가 들어갔다.
- Push는 `빠른 템포`, `측면 진입이 쉬운 porous 구조`, `양 진영을 시각적으로 구분하는 랜드마크`가 중요하다고 설명한다.
- 이 원칙은 우리에게 그대로 유효하다.
- 즉, 처음부터 거대한 맵보다 `명확한 엄폐`, `짧은 교전 복귀`, `읽기 쉬운 목표 지점`, `우회로 1개`가 있는 소형 전투 맵이 낫다.

---

## 지금은 가져오지 말아야 하는 것

### 1. 영웅 수 경쟁
- 공식 사이트 기준 현재 영웅 수는 매우 많고 계속 늘어난다.
- 우리는 초반에 `영웅 1명 완성 -> 2명 확장` 외에는 욕심내지 않는 편이 맞다.

### 2. Stadium 전체 복제
- Stadium은 별도 경제, 라운드, 능력 개조, 3인칭 카메라, 전용 UI까지 포함한 큰 시스템이다.
- 학습용 첫 구현 대상으로는 범위가 너무 크다.
- Stadium은 `후반부 확장용 참고 자료`로만 남기고, 현재 구현 대상에서는 제외한다.

### 3. 라이브 서비스 메타 시스템
- 배틀패스
- 상점
- 크로스플레이 계정 진행
- 공개 프로필 장식
- 시즌 운영

이 항목들은 오버워치 서비스 제품에는 중요하지만, 지금 레포의 학습 목표에는 직접 기여하지 않는다.

---

## 우리 레포 기준 구현 우선순위

### Phase 1. 지금 바로 유지할 오버워치 핵심
- 전용 서버 접속
- 영웅 1명
- 이동
- 기본 공격
- 피해
- 사망
- 리스폰
- 체력 HUD

이것은 이미 `Overwatch-Current-Milestone-01.md`와 일치한다.

### Phase 2. 오버워치다움을 크게 올리는 최소 확장
- 역할 1개를 명확히 가진 영웅 2명
- 히트스캔과 투사체 차이
- 이동기 또는 생존기 1개
- 궁극기 게이지와 궁극기 1개
- 간단한 매치 규칙 1개

처음 게임 모드는 `Team Deathmatch` 또는 매우 단순한 `Control-lite`가 적합하다.  
오버워치의 공식 모드는 다양하지만, 우리 학습 순서에서는 `복제 난이도`보다 `owner/authority 검증`이 우선이다.

### Phase 3. 구조가 안정된 뒤 고려할 것
- Hero Selection
- 역할 잠금
- Escort / Hybrid / Push 계열 목표물
- Flashpoint식 순차 거점
- Perk 또는 모드별 변형 규칙

---

## 시스템 설계에 바로 반영할 포인트

### 입력
- 입력은 영웅 능력 슬롯 기준으로 유지한다.
- 추천 기본 축:
- `IA_Move`
- `IA_Look`
- `IA_Jump`
- `IA_Crouch`
- `IA_PrimaryFire`
- `IA_SecondaryFire`
- `IA_Ability1`
- `IA_Ability2`
- `IA_Ultimate`
- `IA_Interact`
- `IA_Reload`

설명:
- 오버워치류 게임은 무기와 스킬이 영웅별로 달라도 입력 슬롯은 비교적 안정적이다.
- 따라서 `무슨 영웅이냐`보다 `어떤 슬롯을 소비하느냐` 기준 입력 설계가 더 확장성이 좋다.

### GAS
- 영웅별 차이를 데이터와 AbilitySet으로 분리해야 한다.
- 나중에 Perk 같은 변형 규칙을 붙이려면 `중앙 if 분기`보다 `부여/해제 가능한 Ability, GameplayEffect, Modifier` 구조가 유리하다.

### UI / HUD
- 공식 PvP 변경 글은 영웅별 HUD가 중요한 게임플레이 정보를 더 잘 전달하도록 개선했다고 설명한다.
- 따라서 HUD는 예쁘게 만드는 것보다 `전투 의사결정에 필요한 값`을 먼저 표시해야 한다.
- 우선순위:
- `체력`
- `탄약 또는 재장전 상태`
- `스킬 쿨다운`
- `궁극기 게이지`
- `사망/리스폰 상태`

### 맵
- 5v5 기준으로 엄폐가 중요하다.
- 초기 맵 제작 체크리스트:
- `주 교전선 1개`
- `엄폐물 반복 간격이 너무 길지 않을 것`
- `우회로 1개`
- `고지대 1개`
- `목표 위치를 시각적으로 바로 읽을 수 있을 것`

---

## 아키텍처 권장안

### 꼭 유지할 구조
- `PlayerController`는 입력 라우팅과 로컬 UX
- `PlayerState`는 지속 플레이어 상태와 향후 ASC owner
- `Character`는 현재 전투 실행체
- `GameMode`는 서버 규칙과 리스폰
- `GameState`는 매치 공용 상태
- `Widget/ViewModel`은 표시 전용

### 앞으로 추가하면 좋은 타입
- `UOWHeroDefinition`
- `UOWAbilitySet`
- `UOWInputConfig`
- `UOWGameModeDefinition`
- `UOWRespawnComponent` 또는 명확한 리스폰 owner

이 구조를 먼저 잡아두면, 오버워치처럼 영웅과 모드가 늘어나도 중앙 분기문 폭증을 피하기 쉽다.

---

## 법적/제작 방향 메모

이 문단은 공식 자료 인용이 아니라, 위 자료를 바탕으로 한 제작 판단이다.

- 레퍼런스로 삼아야 할 것은 `역할 구조`, `전투 리듬`, `상태 ownership`, `맵 readability`다.
- 그대로 복제하면 안 되는 것은 `영웅 이름`, `세계관`, `대사`, `아트`, `정확한 능력 연출`, `UI 자산`, `브랜드 표현`이다.
- 즉 `오버워치 같은 구조`는 배워도 되지만, `오버워치 그 자체`를 복제하는 방향으로 가면 안 된다.

---

## 우리 프로젝트용 최종 요약

지금 필요한 오버워치 레퍼런스는 아래 한 줄로 요약된다.

`5v5 역할 기반 히어로 슈터의 구조를 목표로 하되, 현재 구현은 전용 서버 기준 영웅 1명 전투 루프를 완성하고, 이후 HeroDefinition + GAS + CommonUI/MVVM으로 확장한다.`

이 기준이면 범위가 과도하게 커지지 않으면서도, 나중에 오버워치다운 확장을 안전하게 이어갈 수 있다.

---

## 출처

1. Overwatch 공식 홈페이지, 확인일 2026-04-19  
   https://overwatch.blizzard.com/en-us/

2. Overwatch 공식 영웅 페이지, 확인일 2026-04-19  
   https://overwatch.blizzard.com/en-us/heroes/

3. Blizzard News, `First look at the player-versus-player changes coming to Overwatch 2`, 2021-05-20  
   https://overwatch.blizzard.com/en-us/news/23663245/

4. Blizzard News, `Uniting gameplay and style: Behind Overwatch 2's complex map design`, 2022-04-07  
   https://overwatch.blizzard.com/en-us/news/23785339/

5. Blizzard News, `Introducing our most exciting season yet... Overwatch 2: Invasion`, 2023-06-12  
   https://overwatch.blizzard.com/en-us/news/23964186/introducing-our-most-exciting-season-yet-overwatch-2-invasion/

6. Blizzard News, `Show Off Your Playstyle with the New Player Progression System`, 2023-08-22  
   https://overwatch.blizzard.com/en-us/news/23991794/

7. Blizzard News, `Overwatch 2 Spotlight: A New Era of Innovation and Excitement`, 2025-02-12  
   https://overwatch.blizzard.com/en-us/news/24177197/

8. Blizzard News, `Enter The Stadium – Get Ready to Battle!`, 2025-04-14  
   https://overwatch.blizzard.com/en-us/news/24188046/

9. Blizzard News, `Introducing the Overwatch Workshop`, 2019-04-24  
   https://overwatch.blizzard.com/en-us/news/22938941/

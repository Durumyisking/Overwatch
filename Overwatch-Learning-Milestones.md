# Overwatch Learning Milestones

## 결론
이 프로젝트는 오버워치 완성이 아니라 `Dedicated Server + GAS + CommonUI + MVVM` 학습을 위한 세로 슬라이스 확장형 프로젝트로 진행한다.

핵심 원칙:
- 한 번에 시스템 하나가 아니라 `플레이 가능한 한 판` 기준으로 자른다.
- 서버 권한, 상태 소유자, UI 관찰자 구조를 먼저 고정한다.
- 새 영웅 추가가 `중앙 분기 수정`이 아니라 `새 타입 / 데이터 추가`로 끝나도록 만든다.

---

## 전체 마일스톤

## Milestone 0. 프로젝트 뼈대와 책임 분리
목표:
- 프로젝트 기본 폴더 구조 확정
- `GameMode / GameState / PlayerState / PlayerController / Character / UI` 책임 분리
- 전용 서버 기준 초기 실행 흐름 정리

완료 조건:
- 각 핵심 클래스의 책임을 문서로 설명할 수 있다.
- 추후 기능 추가 시 어느 클래스가 owner인지 바로 판단할 수 있다.

산출물:
- 기본 클래스 뼈대
- 아키텍처 문서

---

## Milestone 1. 최소 전투 루프
목표:
- 2인 멀티플레이 전용 서버 접속
- 영웅 1명 스폰
- 기본 공격으로 피해 적용
- 사망
- 리스폰
- HUD 체력 표시

핵심 학습:
- 서버 authoritative 전투
- PlayerState / Character 책임 분리
- UI는 상태를 표시만 하는 구조

완료 조건:
- 두 클라이언트에서 서로에게 피해를 줄 수 있다.
- 사망과 리스폰이 모든 클라이언트에서 일관되게 보인다.
- HUD가 현재 체력을 정확히 반영한다.

---

## Milestone 2. GAS 기본 통합
목표:
- ASC owner 구조 확정
- Health AttributeSet 도입
- 기본 공격 Ability 적용
- 쿨다운 1개 스킬 추가

권장 구조:
- `OwnerActor = PlayerState`
- `AvatarActor = Character`

핵심 학습:
- respawn 이후에도 유지되어야 하는 능력 시스템 상태 관리
- Attribute / Effect / Ability 초기화 경로 정리

완료 조건:
- 스폰과 리스폰 이후에도 ASC가 정상 동작한다.
- 공격과 스킬 실행이 GAS 경로로 일관되게 처리된다.

---

## Milestone 3. 영웅 정의와 확장 구조
목표:
- `HeroDefinition` 데이터 구조 설계
- AbilitySet / 무기 설정 / HUD 설정을 데이터로 분리
- 영웅 2명 이상 추가 가능한 구조 정리

핵심 학습:
- 새 영웅 추가 시 중앙 분기문 증가를 피하는 설계
- 데이터 중심 초기화

완료 조건:
- 새 영웅 추가가 `DataAsset + Character + AbilitySet` 단위로 가능하다.
- 영웅별 차이를 데이터와 타입으로 설명할 수 있다.

---

## Milestone 4. 로비와 영웅 선택
목표:
- CommonUI 기반 로비 화면
- 영웅 선택 화면
- 서버 authoritative 영웅 선택 저장
- 선택 결과 기반 스폰

핵심 학습:
- CommonUI로 화면 흐름 제어
- UI intent -> Controller/RPC -> authoritative owner 흐름 확립

완료 조건:
- 각 플레이어가 영웅을 선택할 수 있다.
- 서버가 최종 선택 결과를 소유한다.
- 선택한 영웅으로 정상 스폰된다.

---

## Milestone 5. HUD와 MVVM 정리
목표:
- 체력
- 탄약
- 쿨다운
- 궁극기 게이지
- 점수판
- 리스폰 대기 UI

핵심 학습:
- MVVM으로 gameplay state를 presentation용 데이터로 투영
- UI가 authoritative state를 직접 변경하지 않는 구조

완료 조건:
- HUD가 gameplay owner의 상태를 안정적으로 반영한다.
- Widget이 gameplay state의 source of truth가 아니다.

---

## Milestone 6. 게임 모드 1개 완성
추천:
- 처음은 `Team Deathmatch` 권장

목표:
- 팀 배정
- 스폰 포인트 분리
- 처치 점수 반영
- 매치 타이머
- 승패 처리

핵심 학습:
- GameMode 서버 규칙
- GameState 매치 공유 상태
- PlayerState 플레이어 지속 통계

완료 조건:
- 한 판이 시작, 진행, 종료까지 완결된다.
- 점수와 승패가 모든 클라이언트에서 일관된다.

---

## Milestone 7. 오버워치다운 전투 감각 보강
목표:
- 영웅 2명 이상
- 역할 차이
- 히트스캔 / 투사체 차이
- 이동기 1종
- 궁극기 1종

핵심 학습:
- 영웅별 전투 리듬 차이 설계
- 공통 시스템 위에 서로 다른 능력 조합 얹기

완료 조건:
- 영웅 간 플레이 감각 차이가 명확하다.
- 기존 구조를 크게 깨지 않고 영웅 추가가 가능하다.

---

## 운영 원칙
- 기능은 항상 `owner`, `authority`, `replication`, `UI projection` 순서로 생각한다.
- Widget과 ViewModel은 게임플레이 권한을 가지지 않는다.
- 완료는 설명이 아니라 `실행/검증 명령`으로 판단한다.
- 큰 기능은 항상 `최소 compile-safe 패치`부터 시작한다.

---

## 추천 진행 순서
1. Milestone 1을 완성한다.
2. Milestone 1 구조를 깨지 않고 GAS를 얹는다.
3. 영웅 확장 구조를 만든 뒤 영웅 수를 늘린다.
4. 로비, 영웅 선택, HUD를 UI 계층으로 연결한다.
5. 마지막에 게임 모드와 오버워치다운 감각을 추가한다.

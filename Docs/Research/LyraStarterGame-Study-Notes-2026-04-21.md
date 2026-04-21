# LyraStarterGame Study Notes

## 결론

Lyra의 핵심은 `Experience`가 게임 한 판의 조립 명세가 되고, `GameFeature`가 기능 조각을 활성화하며, `ModularGameplayActors + InitState`가 런타임 초기화 순서를 안전하게 맞추고, `ASC`는 `PlayerState`에 두어 사망과 리스폰을 넘어서 지속성을 유지하는 구조다.

오버워치를 Unreal로 학습용 구현할 때도 이 축을 그대로 가져가는 것이 가장 학습 가치가 높다.

## 이번 학습 범위

전체 프로젝트 트리와 주요 소스 축을 스캔했다.

- `F:\Dev\LyraStarterGame\Source\LyraGame`
- `F:\Dev\LyraStarterGame\Plugins\CommonGame`
- `F:\Dev\LyraStarterGame\Plugins\CommonLoadingScreen`
- `F:\Dev\LyraStarterGame\Plugins\CommonUser`
- `F:\Dev\LyraStarterGame\Plugins\GameplayMessageRouter`
- `F:\Dev\LyraStarterGame\Plugins\ModularGameplayActors`
- `F:\Dev\LyraStarterGame\Plugins\UIExtension`
- `F:\Dev\LyraStarterGame\Plugins\GameFeatures\ShooterCore`
- `F:\Dev\LyraStarterGame\Plugins\GameFeatures\ShooterTests`
- `F:\Dev\LyraStarterGame\Plugins\GameFeatures\TopDownArena`

특히 아래 파일들은 직접 읽고 구조를 해석했다.

- `Source\LyraGame\GameModes\LyraGameMode.*`
- `Source\LyraGame\GameModes\LyraGameState.*`
- `Source\LyraGame\GameModes\LyraExperienceDefinition.*`
- `Source\LyraGame\GameModes\LyraExperienceActionSet.*`
- `Source\LyraGame\GameModes\LyraExperienceManagerComponent.*`
- `Source\LyraGame\GameFeatures\LyraGameFeaturePolicy.*`
- `Source\LyraGame\GameFeatures\GameFeatureAction_AddAbilities.*`
- `Source\LyraGame\GameFeatures\GameFeatureAction_AddWidget.*`
- `Source\LyraGame\Player\LyraPlayerController.*`
- `Source\LyraGame\Player\LyraPlayerState.*`
- `Source\LyraGame\Player\LyraPlayerSpawningManagerComponent.*`
- `Source\LyraGame\Character\LyraPawnExtensionComponent.*`
- `Source\LyraGame\Character\LyraHeroComponent.*`
- `Source\LyraGame\AbilitySystem\LyraAbilitySystemComponent.*`
- `Source\LyraGame\AbilitySystem\LyraAbilitySet.*`
- `Source\LyraGame\AbilitySystem\Phases\LyraGamePhaseSubsystem.*`
- `Source\LyraGame\Teams\LyraTeamSubsystem.*`
- `Source\LyraGame\Equipment\LyraEquipmentManagerComponent.*`
- `Source\LyraGame\Weapons\LyraWeaponInstance.h`
- `Source\LyraGame\Weapons\LyraRangedWeaponInstance.h`
- `Plugins\CommonGame\Source\Public\GameUIManagerSubsystem.h`
- `Plugins\CommonGame\Source\Private\GameUIManagerSubsystem.cpp`
- `Plugins\CommonGame\Source\Public\GameUIPolicy.h`
- `Plugins\CommonGame\Source\Private\GameUIPolicy.cpp`
- `Plugins\CommonGame\Source\Public\PrimaryGameLayout.h`
- `Plugins\CommonGame\Source\Private\PrimaryGameLayout.cpp`
- `Plugins\GameplayMessageRouter\Source\GameplayMessageRuntime\Public\GameFramework\GameplayMessageSubsystem.h`
- `Source\LyraGame\Messages\GameplayMessageProcessor.h`
- `Source\LyraGame\Messages\LyraVerbMessage.h`
- `Source\LyraGame\Messages\LyraVerbMessageReplication.h`
- `Source\LyraServer.Target.cs`
- `Source\LyraGame\System\LyraReplicationGraph.h`

## 모듈 지도

`LyraGame`에서 파일 수가 많은 영역은 아래와 같다.

- `UI`: 79
- `AbilitySystem`: 51
- `Settings`: 35
- `System`: 27
- `Teams`: 22
- `GameModes`: 20
- `Feedback`: 19
- `Interaction`: 17
- `Player`: 16
- `Weapons`: 16
- `Inventory`: 16
- `Character`: 16
- `GameFeatures`: 16

핵심 플러그인 소스 규모는 아래와 같다.

- `GameSettings`: 61
- `CommonGame`: 30
- `ShooterCore`: 27
- `ShooterTests`: 18
- `ModularGameplayActors`: 16
- `CommonLoadingScreen`: 15
- `CommonUser`: 13
- `PocketWorlds`: 12
- `TopDownArena`: 11
- `GameplayMessageRouter`: 11
- `GameSubtitles`: 11
- `UIExtension`: 8

## 구조 원칙

### 1. Experience가 판의 조립 명세다

`ULyraExperienceDefinition`은 단순 데이터 에셋이지만 실질적으로는 한 판의 루트 구성이다.

- 어떤 `GameFeature`를 켤지
- 기본 `PawnData`가 무엇인지
- 어떤 `GameFeatureAction`을 실행할지
- 어떤 `ActionSet`을 조합할지

중요한 점은 `GameMode`에 규칙을 하드코딩하지 않고, `Experience`를 통해 조립한다는 점이다.

오버워치 적용:

- `QP_PayloadExperience`
- `QP_ControlExperience`
- `TrainingRangeExperience`
- `FrontendExperience`

처럼 모드를 `Experience` 단위로 나누는 것이 맞다.

### 2. GameMode는 오케스트레이션만 한다

`ALyraGameMode`는 직접 많은 규칙을 소유하지 않는다.

- 경험치 아님, 경험치가 아니라 `Experience`를 선택한다
- 월드/옵션/커맨드라인/전용서버 상태를 보고 경험을 결정한다
- 플레이어 리스타트 흐름을 연결한다
- 실제 초기화는 `GameState`의 `ExperienceManagerComponent`가 맡는다

즉 `GameMode`는 서버 전용 판 관리자이고, 구체 규칙 소유자가 아니다.

오버워치 적용:

- `OWGameMode`에 영웅별 분기, 무기별 분기, UI 분기 넣지 말 것
- `GameMode`는 매치 시작, 경험 선택, 리스폰 요청, 세션 호스팅만 담당

### 3. GameStateComponent가 실제 런타임 규칙 허브다

Lyra는 `UGameStateComponent`를 매우 적극적으로 쓴다.

- `ULyraExperienceManagerComponent`
- `ULyraPlayerSpawningManagerComponent`
- 팀 생성/봇 생성/프론트엔드 상태 같은 확장

이 패턴의 장점:

- 매치 전역 규칙을 `GameState` 기준으로 복제 친화적으로 붙일 수 있다
- 경험마다 필요한 전역 기능을 교체/추가하기 쉽다
- `GameMode` 비대화를 막는다

오버워치 적용:

- `OWRespawnManagerComponent`
- `OWObjectiveStateComponent`
- `OWHeroSelectStateComponent`
- `OWMatchFlowComponent`

같은 식으로 나누는 방향이 좋다.

### 4. ASC는 PlayerState에 두고 Pawn은 Avatar로만 쓴다

Lyra에서 가장 중요한 학습 포인트 중 하나다.

- `ALyraPlayerState`가 `ULyraAbilitySystemComponent`를 소유
- `ULyraPawnExtensionComponent`가 현재 Pawn을 ASC의 Avatar로 연결
- 리스폰 시 Avatar만 교체
- 지속돼야 하는 능력/속성/태그는 `PlayerState` 기준 유지

이 구조는 영웅 슈터에 특히 잘 맞는다.

오버워치 적용:

- 궁극기 게이지
- 영웅 상태 이상 저항
- 플레이어 소유의 영웅 로드아웃
- 팀/스쿼드/역할 태그

같이 리스폰 이후에도 남아야 할 것은 `PlayerState` 중심으로 두는 것이 안정적이다.

### 5. PawnExtension이 초기화 조율자다

`ULyraPawnExtensionComponent`는 단순 헬퍼가 아니다. 초기화 순서를 통제하는 코디네이터다.

- `PawnData`가 왔는지
- 컨트롤러가 붙었는지
- `PlayerState`가 복제되었는지
- ASC가 준비되었는지
- 다른 피처 컴포넌트들이 `DataAvailable`, `DataInitialized`까지 갔는지

즉 "초기화 순서 문제"를 엔진 이벤트 여기저기 분산시키지 않고 컴포넌트 상태 기계로 정리한다.

오버워치 적용:

- `HeroComponent`
- `WeaponComponent`
- `CameraModeComponent`
- `UIBridgeComponent`

가 제각각 `BeginPlay`, `PossessedBy`, `OnRep_PlayerState`에서 따로 놀지 말고, `PawnExtension`류에서 상태 전이를 기준으로 묶는 것이 좋다.

### 6. HeroComponent는 로컬 플레이어용 입력/카메라 조립자다

`ULyraHeroComponent`의 책임은 명확하다.

- 입력 초기화
- 입력 태그를 ASC로 전달
- 추가 입력 설정 적용
- 카메라 모드 결정

중요한 점은 `HeroComponent`가 게임 규칙을 소유하지 않는다는 점이다.
입력과 카메라 오케스트레이션만 한다.

오버워치 적용:

- 영웅 스킬 입력은 `InputTag` 기반
- 실제 발동 규칙은 능력에서 처리
- `HeroComponent`는 입력 라우팅과 카메라 모드 전환만 담당

### 7. AbilitySet은 영웅/장비 조립 단위다

`ULyraAbilitySet`은 매우 실용적이다.

- 능력 부여
- GameplayEffect 부여
- AttributeSet 부여
- 제거 핸들 보관

핵심은 영웅/장비/경험이 직접 ASC 내부 구현을 몰라도 된다는 점이다.

오버워치 적용:

- 영웅 기본 능력 세트
- 무기 능력 세트
- 패시브 능력 세트
- 게임 모드 공통 능력 세트

를 데이터 자산으로 분리해야 한다.

### 8. GameFeatureAction은 기능의 주입 경계다

Lyra는 기능 추가를 중앙 `switch`가 아니라 액션 타입 확장으로 푼다.

- `AddAbilities`
- `AddWidget`
- `AddInputBinding`
- `AddInputContextMapping`
- `AddGameplayCuePath`

이게 객체지향적으로 좋은 이유는 "새 기능 추가"가 "기존 분기 수정"이 아니라 "새 액션 타입 추가"가 되기 때문이다.

오버워치 적용:

- 신규 영웅 추가 시 중앙 로직 수정 최소화
- 신규 모드 UI 추가 시 HUD 하드코딩 제거
- 신규 무기 추가 시 경험/장비 데이터만 조합

### 9. CommonGame + UIExtension이 UI 조립 기반이다

UI 구조도 하드코딩보다 레이어/슬롯 조립에 가깝다.

- `UGameUIManagerSubsystem`
- `UGameUIPolicy`
- `UPrimaryGameLayout`
- `UUIExtensionSubsystem`
- `GameFeatureAction_AddWidgets`

이 조합으로

- HUD 루트 레이아웃 생성
- 레이어별 위젯 푸시
- 슬롯 태그 기반 UI 삽입

을 구현한다.

오버워치 적용:

- 고정 HUD를 한 위젯에 다 넣지 말 것
- `Reticle`, `AbilityBar`, `ObjectiveBanner`, `KillFeed`, `DamageNumbers`, `HeroPortrait`를 레이어/슬롯으로 분리
- 영웅별 UI는 GameFeature 또는 HeroData 기반으로 주입

### 10. 메시지는 직접 참조보다 GameplayMessageRouter를 우선한다

Lyra는 클라이언트 알림성 이벤트를 메시지 라우터로 보낸다.

- `FLyraVerbMessage`
- `UGameplayMessageSubsystem`
- `UGameplayMessageProcessor`
- `FLyraVerbMessageReplication`

이 패턴이 좋은 이유:

- 킬피드, 어시스트, 연속 처치, 알림 토스트가 서로 직접 참조하지 않는다
- 서버 측 프로세서가 메시지를 듣고 2차 메시지를 만들 수 있다
- UI는 메시지를 구독만 하면 된다

오버워치 적용:

- `Elimination`
- `Assist`
- `ObjectiveCaptured`
- `PayloadCheckpointReached`
- `UltimateReady`

등을 메시지 채널로 흘리는 것이 좋다.

### 11. GamePhase는 GAS 태그 기반 매치 흐름 관리다

`ULyraGamePhaseSubsystem`은 단순 enum 상태 머신보다 확장성이 좋다.

- `Game.Playing`
- `Game.Playing.WarmUp`
- `Game.Playing.Overtime`
- `Game.PostGame`

같은 계층형 태그를 허용한다.

오버워치 적용:

- `Game.Frontend`
- `Game.Match.PreGame`
- `Game.Match.AssembleHeroes`
- `Game.Match.InProgress`
- `Game.Match.Overtime`
- `Game.Match.PostGame`

처럼 단계 태그를 설계하면 좋다.

### 12. TeamSubsystem은 팀 규칙 접근을 한 곳으로 모은다

Lyra는 팀 계산을 여러 클래스가 직접 하지 않는다.

- 팀 비교
- 아군/적군 판정
- 팀 태그
- 팀 표시 자산

을 `ULyraTeamSubsystem`에 모은다.

오버워치 적용:

- 힐 가능 여부
- 피해 허용 여부
- 아군 아웃라인/색상
- 관전자 시점 팀 색상 처리

를 서브시스템 중심으로 모아야 한다.

### 13. 장비와 무기는 "데이터 정의 + 인스턴스 런타임" 조합이다

`EquipmentDefinition -> EquipmentInstance -> WeaponInstance` 구조가 깔끔하다.

- 정의는 데이터 자산
- 런타임 상태는 인스턴스 객체
- 부여 효과는 AbilitySet
- 복제는 FastArray + SubObject

오버워치 적용:

- 영웅 기본 무기
- 임시 버프 장비
- 설치물 생성기
- 투사체 발사기

를 액터 한 클래스에 뭉개지 말고 장비 인스턴스로 관리하는 것이 좋다.

### 14. 전용 서버 흐름이 프런트엔드와 분리되어 있다

`ALyraGameMode::TryDedicatedServerLogin`과 `HostDedicatedServerMatch`는
전용 서버가 기본 맵에서 기동된 뒤 온라인 로그인/세션 호스팅/경험 선택으로 들어가는 흐름을 보여준다.

오버워치 적용:

- 전용 서버 실행 시 바로 프런트엔드 로직 타지 않게 분리
- 서버 시작 파라미터로 모드/맵/플레이리스트 선택 가능하게 설계
- `FrontendExperience`와 `DedicatedServerMatchExperience`를 의도적으로 분리

### 15. ReplicationGraph와 FastArray를 적극 활용한다

Lyra는 대규모 플레이어/액터를 염두에 두고 있다.

- `ULyraReplicationGraph`
- PlayerState 빈도 제한 노드
- 장비/메시지 복제의 FastArray 사용

오버워치 적용:

- 궁극기 이펙트, 투사체, 장비, 상태 메세지에 대해 기본 복제만 믿지 말 것
- 수명이 짧고 개수가 많은 객체는 RepGraph/NetCull/FastArray 조합을 검토할 것

## 객체지향 / C++ 학습 포인트

### 책임 분리

- `GameMode`: 판 시작 결정
- `GameStateComponent`: 전역 규칙 실행
- `PlayerState`: 지속 상태와 ASC 소유
- `Pawn`: 현재 몸체
- `PawnExtension`: 초기화 코디네이션
- `HeroComponent`: 입력/카메라 조립
- `AbilitySet`: 부여 묶음
- `GameFeatureAction`: 기능 주입

한 클래스가 모든 것을 아는 구조를 피하고 있다.

### 조합 우선

Lyra는 상속보다 조합을 많이 쓴다.

- `Component`
- `Subsystem`
- `DataAsset`
- `GameFeatureAction`

를 이용해 기능을 붙인다.

오버워치처럼 영웅/무기/모드 조합이 많은 게임은 이 방향이 훨씬 낫다.

### 확장 포인트를 타입으로 만든다

중앙 분기문 대신 새 타입을 추가하는 방식이 많다.

- 새 `Experience`
- 새 `AbilitySet`
- 새 `GameFeatureAction`
- 새 `EquipmentDefinition`
- 새 `MessageProcessor`

이것이 유지보수성을 높인다.

### 핸들 기반 회수

부여한 것을 나중에 안전하게 회수하기 위해 핸들을 저장한다.

- `FGameplayAbilitySpecHandle`
- `FActiveGameplayEffectHandle`
- `FLyraAbilitySet_GrantedHandles`
- `FGameplayMessageListenerHandle`
- `FUIExtensionHandle`

학습 포인트는 "추가만큼 제거도 설계에 포함"해야 한다는 점이다.

## Unreal Engine 사용 기법 학습 포인트

### GameFrameworkComponentManager + InitState

초기화 순서 꼬임을 엔진 생명주기 이벤트 남발이 아니라 상태 태그 체인으로 해결한다.

### PrimaryDataAsset + AssetManager + Bundle

경험, 액션 세트, 능력 세트를 에셋으로 관리하고 번들 상태로 로드 대상을 나눈다.

### Dedicated Server 분기

- `IsDedicatedServerInstance`
- `NM_DedicatedServer`
- `LoadStateClient`, `LoadStateServer`

같이 클라이언트/서버 자산 로드를 분리한다.

### CommonUI 레이어 구조

UI는 액터가 아니라 레이어 스택과 슬롯 삽입으로 조립한다.

### Gameplay Tags를 상태 언어로 사용

입력, 능력 차단, 게임 단계, 메시지 채널 등 많은 축을 태그로 통일한다.

### FastArraySerializer

자주 바뀌는 리스트형 복제를 직접 구조화한다.

### Registered SubObject Replication

장비 인스턴스 같은 UObject 런타임 오브젝트도 복제 대상으로 다룬다.

## MVVM 관찰

이번 C++ 소스 기준으로는 MVVM이 강하게 드러나지 않는다.
Lyra의 실제 UI 데이터 바인딩은 상당 부분 블루프린트/위젯 자산/데이터 중심으로 풀린다.

따라서 오버워치 프로젝트에서는 다음 원칙으로 해석하는 것이 좋다.

- C++은 상태 소유자와 투영 API를 제공
- ViewModel은 UI 전용 투영 객체로 제한
- 위젯이 ASC, PlayerState, GameState를 직접 만지지 않게 중간 계층을 둔다

즉 Lyra가 주는 직접 학습 포인트는 "MVVM 클래스 그 자체"보다
"UI를 소유권 밖에서 관찰하게 만드는 구조"에 가깝다.

## 오버워치 프로젝트에 바로 적용할 규칙

### 최우선

- ASC는 `PlayerState` 소유로 유지
- Pawn은 Avatar 중심으로 운용
- 영웅/모드는 `Experience` 기반으로 조립
- UI는 `CommonUI + UIExtension` 레이어 구조로 조립
- 입력은 `InputTag` 중심으로 바인딩
- 매치 흐름은 `GamePhase` 태그 체계로 관리

### 그다음

- 무기/스킬은 `AbilitySet + EquipmentInstance` 구조로 분리
- 게임 전역 규칙은 `GameStateComponent`로 이동
- 알림/킬피드/어시스트는 `GameplayMessageRouter` 기반으로 분리
- 스폰 규칙은 전용 `SpawningManagerComponent`에 위임

### 피해야 할 것

- 위젯이 게임플레이 상태를 직접 소유하는 것
- `GameMode`에 영웅/무기/스폰/UI 규칙을 몰아넣는 것
- `switch(HeroType)` 중앙 분기 확산
- Pawn과 PlayerState에 동일한 진실을 중복 저장하는 것
- 리스폰마다 ASC를 새로 만드는 것

## 우리 프로젝트용 권장 골격

### 서버/판 전역

- `AOWGameMode`: 경험 선택, 매치 시작/종료, 리스타트 요청
- `AOWGameState`: 복제 가능한 판 상태
- `UOWExperienceManagerComponent`
- `UOWMatchFlowComponent`
- `UOWObjectiveStateComponent`
- `UOWPlayerSpawningManagerComponent`

### 플레이어/영웅

- `AOWPlayerState`: ASC, 영웅 선택, 궁 게이지, 팀 정보
- `AOWHeroPawn`: 현재 몸체
- `UOWPawnExtensionComponent`
- `UOWHeroComponent`
- `UOWEquipmentManagerComponent`

### 데이터

- `UOWExperienceDefinition`
- `UOWHeroDefinition`
- `UOWPawnData`
- `UOWAbilitySet`
- `UOWEquipmentDefinition`
- `UOWWeaponDefinition`

### UI

- `UOWUIManagerSubsystem`
- `UOWUIPolicy`
- `UOWPrimaryGameLayout`
- `UIExtension` 슬롯 기반 HUD
- 필요 시 영웅별 ViewModel 계층 추가

## 다음에 깊게 이어서 볼 가치가 큰 영역

- `ShooterCore`의 사격/피격/점수 처리 전체
- `Inventory`와 `QuickBar` 흐름
- `Interaction`과 `WorldCollectable`
- `GameFeatureAction_AddInputBinding`, `AddInputContextMapping`
- `TopDownArena`, `ShooterTests`가 보여주는 확장 예시
- 블루프린트 에셋 쪽 `Experience`, `PawnData`, `AbilitySet`, `HUD Layout` 실제 데이터 구성

## 메모

이 문서는 "Lyra 구조를 Overwatch 학습 프로젝트에 재사용하기 위한 기준 문서"다.
향후 구현 시에는 이 문서를 기준으로 현재 `OW*` 구조와 `Hak/Lyra` 구조가 충돌하는 지점을 비교하고,
가능하면 `Hak/Lyra` 소유권 모델을 우선한다.

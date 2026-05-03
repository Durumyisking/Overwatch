#include "Character/Components/OWPawnExtensionComponent.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Core/Types/OWGameplayTags.h"
#include "Data/OWPawnData.h"
#include "OWLog.h"
#include "Net/UnrealNetwork.h"

const FName UOWPawnExtensionComponent::NAME_ActorFeatureName(TEXT("PawnExtension"));

UOWPawnExtensionComponent::UOWPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetIsReplicatedByDefault(true);
}

void UOWPawnExtensionComponent::SetPawnData(const UOWPawnData* InPawnData)
{
	// Pawn이 Authority가 아닌 경우 SetPawnData를 허용하지 않는다.
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData || !InPawnData)
	{
		return;
	}

	// PawnData 업데이트
	PawnData = InPawnData;
	Pawn->ForceNetUpdate();
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::InitializeAbilitySystem(UOWAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		return;
	}

	if (AbilitySystemComponent)
	{
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	if (AActor* ExistingAvatar = InASC->GetAvatarActor())
	{
		if (ExistingAvatar != Pawn)
		{
			// 이미 다른 Pawn이 ASC의 AvatarActor라면 기존 PawnExtension을 통해 연결을 끊는다.
			// 클라이언트 지연으로 새 Pawn이 먼저 생성/possess되고 이전 Pawn 제거가 늦는 경우를 처리한다.
			if (UOWPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
			{
				OtherExtensionComponent->UninitializeAbilitySystem();
			}
		}
	}

	// ASC를 캐시하고 InitAbilityActorInfo를 Pawn 기준으로 다시 호출하여 AvatarActor를 현재 Pawn으로 갱신한다.
	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	// OnAbilitySystemInitialized에 바인딩된 Delegate를 호출한다.
	OnAbilitySystemInitialized.Broadcast();
}

void UOWPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		// 이 Pawn이 아직 ASC의 AvatarActor라면 입력 상태를 비우고 Avatar 연결을 끊는다.
		AbilitySystemComponent->ClearAbilityInput();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		// OnAbilitySystemUninitialized에 바인딩된 Delegate를 호출한다.
		OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UOWPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>())
	{
		if (AbilitySystemComponent->GetOwnerActor())
		{
			// Controller가 바뀌었으므로 ASC의 ActorInfo를 새 Controller/Avatar 상태에 맞게 갱신한다.
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
		else
		{
			UninitializeAbilitySystem();
		}
	}

	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::SetupPlayerInputComponent()
{
	// 입력 컴포넌트 준비는 InitState 체인을 다시 진행할 수 있는 조건이다.
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		OW_ERROR(LogOWGame, "OWPawnExtensionComponent requires a Pawn owner.");
		return;
	}

	// GameFrameworkComponentManager의 InitState 시스템에 이 Feature를 등록한다.
	RegisterInitStateFeature();
}

void UOWPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	// NAME_None은 Actor에 등록된 모든 Feature Component의 InitState 변화를 관찰하겠다는 의미다.
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	// Spawned 상태로 진입한 뒤, 나머지 기본 초기화를 이어서 시도한다.
	ensure(TryToChangeInitState(FOWGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UOWPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UOWPawnExtensionComponent, PawnData);
}

void UOWPawnExtensionComponent::OnRep_PawnData()
{
	// Replicated PawnData is the data-availability gate for client-side feature initialization.
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == NAME_ActorFeatureName)
	{
		return;
	}

	// 다른 Feature Component가 DataAvailable에 도달하면 PawnExtension도 DataInitialized로 갈 수 있는지 다시 확인한다.
	if (Params.FeatureState == FOWGameplayTags::Get().InitState_DataAvailable)
	{
		CheckDefaultInitialization();
	}
}

bool UOWPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) const
{
	check(InManager);

	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!InCurrentState.IsValid() && InDesiredState == GameplayTags.InitState_Spawned)
	{
		// Pawn이 존재하면 Spawned 상태로 전환할 수 있다.
		return Pawn != nullptr;
	}

	if (InCurrentState == GameplayTags.InitState_Spawned && InDesiredState == GameplayTags.InitState_DataAvailable)
	{
		// PawnData는 이 Pawn의 클래스, 입력, 카메라, Ability 구성의 기준 데이터다.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn && Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn && Pawn->IsLocallyControlled();
		if ((bHasAuthority || bIsLocallyControlled) && !GetController<AController>())
		{
			// Authority 또는 로컬 제어 Pawn은 Controller possession이 끝날 때까지 기다린다.
			return false;
		}

		return true;
	}

	if (InCurrentState == GameplayTags.InitState_DataAvailable && InDesiredState == GameplayTags.InitState_DataInitialized)
	{
		// Actor의 모든 Feature가 DataAvailable에 도달하면 DataInitialized로 전환한다.
		return InManager->HaveAllFeaturesReachedInitState(Pawn, GameplayTags.InitState_DataAvailable);
	}

	if (InCurrentState == GameplayTags.InitState_DataInitialized && InDesiredState == GameplayTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UOWPawnExtensionComponent::CheckDefaultInitialization()
{
	// 진행 상황을 확인하기 전에 이 Actor의 다른 Feature들도 먼저 진행시킨다.
	CheckDefaultInitializationForImplementers();

	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { GameplayTags.InitState_Spawned, GameplayTags.InitState_DataAvailable, GameplayTags.InitState_DataInitialized, GameplayTags.InitState_GameplayReady };

	// Spawned부터 GameplayReady까지 선형 상태 체인을 가능한 만큼 진행한다.
	ContinueInitStateChain(StateChain);
}

void UOWPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate InDelegate)
{
	// OnAbilitySystemInitialized에 UObject delegate를 중복 없이 추가한다.
	if (!OnAbilitySystemInitialized.IsBoundToObject(InDelegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(InDelegate);
	}

	// 이미 ASC가 초기화되어 있다면 등록만 하지 않고 즉시 호출한다.
	if (AbilitySystemComponent)
	{
		InDelegate.Execute();
	}
}

void UOWPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate InDelegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(InDelegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(InDelegate);
	}
}

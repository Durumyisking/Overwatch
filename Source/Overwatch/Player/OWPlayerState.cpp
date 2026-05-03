#include "OWPlayerState.h"

#include "AbilitySystem/OWAbilitySet.h"
#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "Data/OWPawnData.h"
#include "Game/OWExperienceDefinition.h"
#include "Game/OWExperienceManagerComponent.h"
#include "Game/OWGameMode.h"
#include "Net/UnrealNetwork.h"

AOWPlayerState::AOWPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOWAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
}

void AOWPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		const AGameStateBase* GameState = World->GetGameState();
		if (GameState)
		{
			if (UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>())
			{
				ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
			}
		}
	}

	if (!AbilitySystemComponent)
	{
		check(AbilitySystemComponent);
		return;
	}

	// 처음 InitAbilityActorInfo를 호출하면 OwnerActor와 AvatarActor가 모두 PlayerState가 된다.
	// 이후 PawnExtensionComponent가 Pawn을 AvatarActor로 다시 연결한다.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AOWPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOWPlayerState, PawnData);
}

UOWAbilitySystemComponent* AOWPlayerState::GetAbilitySystemComponent() const
{
	return GetOWAbilitySystemComponent();
}

UOWAbilitySystemComponent* AOWPlayerState::GetOWAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AOWPlayerState::OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience)
{
	AOWGameMode* GameMode = GetWorld() ? GetWorld()->GetAuthGameMode<AOWGameMode>() : nullptr;
	if (!GameMode)
	{
		return;
	}

	// GameMode의 GetPawnDataForController를 통해 PlayerState override 또는 Experience 기본 PawnData를 가져온다.
	const UOWPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
	if (NewPawnData)
	{
		SetPawnData(NewPawnData);
	}
}

void AOWPlayerState::SetPawnData(const UOWPawnData* InPawnData)
{
	if (!InPawnData || GetLocalRole() != ROLE_Authority || PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	// PawnData의 AbilitySet을 순회하며 ASC에 Ability를 부여한다.
	// 이 단계에서 ASC의 ActivatableAbilities에 GameplayAbilitySpec이 추가된다.
	for (UOWAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}

	ForceNetUpdate();
}

void AOWPlayerState::OnRep_PawnData()
{
}

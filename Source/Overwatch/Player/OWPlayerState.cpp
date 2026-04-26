#include "OWPlayerState.h"

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

	const AGameStateBase* GameState = GetWorld() ? GetWorld()->GetGameState() : nullptr;
	if (GameState)
	{
		if (UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>())
		{
			ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
		}
	}

	if (!AbilitySystemComponent)
	{
		check(AbilitySystemComponent);
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AOWPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AOWPlayerState, PawnData);
}

UOWAbilitySystemComponent* AOWPlayerState::GetAbilitySystemComponent() const
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

	const UOWPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController());
	if (NewPawnData)
	{
		SetPawnData(NewPawnData);
	}
}

void AOWPlayerState::SetPawnData(const UOWPawnData* InPawnData)
{
	if (!InPawnData || PawnData)
	{
		return;
	}

	PawnData = InPawnData;
}

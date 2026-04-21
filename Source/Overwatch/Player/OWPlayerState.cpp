#include "OWPlayerState.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "AbilitySystem/OWAbilitySet.h"
#include "Data/OWHeroData.h"
#include "Data/OWPawnData.h"
#include "Game/OWExperienceDefinition.h"
#include "Game/OWExperienceManagerComponent.h"
#include "Game/OWGameMode.h"

AOWPlayerState::AOWPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOWAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
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

void AOWPlayerState::SetHeroData(const FOWHeroData* InHeroData)
{
	if (!AbilitySystemComponent)
	{
		return;
	}

	if (!InHeroData)
	{
		return;
	}

	UOWAbilitySet* NewAbilitySet = InHeroData->AbilitySet;
	if (AppliedAbilitySet == NewAbilitySet)
	{
		return;
	}

	if (AppliedAbilitySet)
	{
		GrantedAbilityHandles.TakeFromAbilitySystem(AbilitySystemComponent);
		GrantedAbilityHandles = FOWAbilitySet_GrantedHandles();
		AppliedAbilitySet = nullptr;
	}

	if (NewAbilitySet)
	{
		NewAbilitySet->GiveAbilitySystem(AbilitySystemComponent, &GrantedAbilityHandles);
		AppliedAbilitySet = NewAbilitySet;
	}
}

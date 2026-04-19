#include "OWPlayerState.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "AbilitySystem/OWAbilitySet.h"
#include "Data/OWHeroData.h"

AOWPlayerState::AOWPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UOWAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

void AOWPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

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

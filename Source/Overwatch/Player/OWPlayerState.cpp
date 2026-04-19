#include "OWPlayerState.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTypes.h"
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


	// 처음 InitAbilityActorInfo를 호출 당시 OwnerActor와 AvatarActor는 모두 같은 액터(PlayerState)를 가리키고 있다.
	// OwnerActor는 액터의 소유자를 나타내며, AvatarActor는 액터가 실제로 행동하는 캐릭터를 나타낸다.
	// 이를 위해 아래처럼 세팅한다.
	if (FGameplayAbilityActorInfo* ActorInfo = AbilitySystemComponent->AbilityActorInfo.Get())
	{
		check(ActorInfo->OwnerActor == this);
		check(ActorInfo->OwnerActor == ActorInfo->AvatarActor);

		AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
	}
}

void AOWPlayerState::SetHeroData(const FOWHeroData* InHeroData)
{
	if (!InHeroData)
	{
		return;
	}
	
	if (UOWAbilitySet* AbilitySet = InHeroData->AbilitySet)
	{
		//AbilitySet->
	}
}

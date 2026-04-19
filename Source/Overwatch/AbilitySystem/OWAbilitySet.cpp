// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OWAbilitySet.h"

#include "OWAbilitySystemComponent.h"
#include "Abilities/OWGameplayAbility.h"


void FOWAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FOWAbilitySet_GrantedHandles::TakeFromAbilitySystem(UOWAbilitySystemComponent* InASC) const
{
	if (!InASC)
	{
		return;
	}

	if (!InASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitySpecHandles)
	{
		if (AbilitySpecHandle.IsValid())
		{
			InASC->ClearAbility(AbilitySpecHandle);
		}
	}
}

void UOWAbilitySet::GiveAbilitySystem(UOWAbilitySystemComponent* InASC, FOWAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	if (!InASC)
	{
		return;
	}

	if (!InASC->IsOwnerActorAuthoritative())
	{
		return;
	}


	// GA를 허용

	// 컴포넌트에 데이터 내 모든 능력을 부여한다.
	for (int32 AbilityIndex = 0; AbilityIndex < Abilities.Num(); ++AbilityIndex)
	{
		const FOWAbilitySet_GameplayAbility& AbilityToGrant = Abilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		// GiveAbility에서 만약 EGameplayAbilityInstancingPolicy가 InstancePerActor인 경우, 내부적으로 Instance를 생성하지만
		// 그렇지 않으면 CDO에서 GetDefaultObject를 통해 객체를 생성한다.
		// - 이를 통해 UObject개수를 줄여 부하 줄일 수 있다.

		// CCProject에서 InstancedStruct를와 CDO로직을 병행했던 것이랑 비슷한 구조라고 볼 수 있겠다.
		UOWGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UOWGameplayAbility>();

		// FGameplayAbilitySpec는 AbilityCDO능력을 부여할건데 어떤 정보를 가지고 부여할것인지 정의
		// AbilitySpec은 GiveAbility로 전달되어 ActivatableAbilities에 추가된다.
		FGameplayAbilitySpec Spec(AbilityCDO);
		Spec.SourceObject = SourceObject;
		Spec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag); // 이 능력은 어떤 태그랑 매치시킬거야~ 라고 명시


		// 여담으로 언리얼에서는 항상 Handle을 사용한다.
		// 이는 객체의 직접 비교를 막고 int데이터로만 비교를 하는게 더 가볍기 때문이다)
		const FGameplayAbilitySpecHandle AbilitySpecHandle = InASC->GiveAbility(Spec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}

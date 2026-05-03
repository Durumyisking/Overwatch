// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OWAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/OWGameplayAbility.h"
#include "GameFramework/Pawn.h"

UOWAbilitySystemComponent::UOWAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOWAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	check(ActorInfo);
	check(InOwnerActor);

	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && InAvatarActor != ActorInfo->AvatarActor;
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (!bHasNewPawnAvatar)
	{
		return;
	}

	// 새 Pawn Avatar가 설정되면 OnSpawn 정책 Ability를 즉시 활성화할 수 있는지 확인한다.
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UOWGameplayAbility* AbilityCDO = Cast<UOWGameplayAbility>(AbilitySpec.Ability))
		{
			if (AbilityCDO->GetActivationPolicy() == EOWAbilityActivationPolicy::OnSpawn)
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UOWAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
	{
		return;
	}

	// GameplayAbilitySpec을 순회하며 입력 태그와 매칭되는 Ability를 Pressed/Held 큐에 넣는다.
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
		{
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
	}
}

void UOWAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
	{
		return;
	}

	// Released 큐에 추가하고 Held 큐에서는 제거한다.
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InInputTag))
		{
			InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void UOWAbilitySystemComponent::ProcessAbilityInput(float InDeltaTime, bool bInGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// InputHeldSpecHandles에서 WhileInputActive 정책 Ability를 찾아 활성화 대상으로 모은다.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		// ActivatableAbilities에서 Handle로 GameplayAbilitySpec을 찾는다.
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UOWGameplayAbility* AbilityCDO = CastChecked<UOWGameplayAbility>(AbilitySpec->Ability);
				if (AbilityCDO->GetActivationPolicy() == EOWAbilityActivationPolicy::WhileInputActive)
				{
					// ActivationPolicy가 WhileInputActive이면 입력이 유지되는 동안 활성화를 시도한다.
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	// 이번 프레임에 Pressed된 Ability를 처리한다.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
				if (AbilitySpec->IsActive())
				{
					// 이미 활성화된 Ability라면 InputPressed 이벤트를 전달한다.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UOWGameplayAbility* AbilityCDO = CastChecked<UOWGameplayAbility>(AbilitySpec->Ability);
					if (AbilityCDO->GetActivationPolicy() == EOWAbilityActivationPolicy::OnInputTriggered)
					{
						// ActivationPolicy가 OnInputTriggered이면 이번 Press에서 활성화를 시도한다.
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// 모아둔 Ability를 한 번에 활성화한다.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// 이번 프레임에 Released된 Ability를 처리한다.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
				if (AbilitySpec->IsActive())
				{
					// 활성화된 Ability라면 InputReleased 이벤트를 전달한다.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	// Held는 Released가 들어올 때 제거되므로 프레임 큐인 Pressed/Released만 비운다.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UOWAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

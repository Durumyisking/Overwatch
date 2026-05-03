// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "OWAbilitySystemComponent.generated.h"

/** 프로젝트 전용 AbilitySystemComponent 확장 지점 */
UCLASS()
class OVERWATCH_API UOWAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UOWAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	void AbilityInputTagPressed(const FGameplayTag& InInputTag);
	void AbilityInputTagReleased(const FGameplayTag& InInputTag);
	void ProcessAbilityInput(float InDeltaTime, bool bInGamePaused);
	void ClearAbilityInput();

private:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;
};

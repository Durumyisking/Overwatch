// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OWGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EOWAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnSpawn,
};

/** OW 프로젝트의 모든 GameplayAbility가 공통으로 상속할 기본 Ability 클래스 */
UCLASS()
class OVERWATCH_API UOWGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UOWGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	EOWAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Ability Activation")
	EOWAbilityActivationPolicy ActivationPolicy = EOWAbilityActivationPolicy::OnInputTriggered;
};

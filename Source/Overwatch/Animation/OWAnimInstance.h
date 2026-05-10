// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "OWAnimInstance.generated.h"

class UAbilitySystemComponent;

/** ASC의 GameplayTag 상태를 애니메이션 블루프린트 변수로 투영하는 프로젝트 기본 AnimInstance다. */
UCLASS(Config = Game)
class OVERWATCH_API UOWAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	UOWAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:

	// 애니메이션 블루프린트가 ASC를 직접 질의하지 않도록 GameplayTag를 변수로 투영한다.
	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
	float GroundDistance = -1.0f;
};

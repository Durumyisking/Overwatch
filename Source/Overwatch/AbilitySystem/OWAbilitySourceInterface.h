#pragma once

#include "UObject/Interface.h"
#include "OWAbilitySourceInterface.generated.h"

class UPhysicalMaterial;
struct FGameplayTagContainer;

/** GameplayEffect 계산에서 거리/피격 재질 보정값을 제공하는 소스 인터페이스다. */
UINTERFACE()
class OVERWATCH_API UOWAbilitySourceInterface : public UInterface
{
	GENERATED_BODY()
};

class OVERWATCH_API IOWAbilitySourceInterface
{
	GENERATED_BODY()

public:
	virtual float GetDistanceAttenuation(float InDistance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};

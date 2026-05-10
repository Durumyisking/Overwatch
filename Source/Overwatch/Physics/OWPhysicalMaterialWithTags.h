#pragma once

#include "GameplayTagContainer.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "OWPhysicalMaterialWithTags.generated.h"

/** 피격 부위나 재질별 데미지 보정을 태그로 표현하는 PhysicalMaterial 확장 타입이다. */
UCLASS()
class OVERWATCH_API UOWPhysicalMaterialWithTags : public UPhysicalMaterial
{
	GENERATED_BODY()

public:
	UOWPhysicalMaterialWithTags(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = PhysicalProperties)
	FGameplayTagContainer Tags;
};

#pragma once

#include "Engine/DataAsset.h"
#include "OWExperienceActionSet.generated.h"

class UGameFeatureAction;

UCLASS()
class OVERWATCH_API UOWExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOWExperienceActionSet();

	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};

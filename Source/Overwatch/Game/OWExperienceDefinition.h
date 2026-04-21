#pragma once

#include "Engine/DataAsset.h"
#include "OWExperienceDefinition.generated.h"

class UGameFeatureAction;
class UOWExperienceActionSet;
class UOWPawnData;

UCLASS()
class OVERWATCH_API UOWExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOWExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<UOWPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<TObjectPtr<UOWExperienceActionSet>> ActionSets;

	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};

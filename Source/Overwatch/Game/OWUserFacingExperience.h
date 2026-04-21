#pragma once

#include "Engine/DataAsset.h"
#include "OWUserFacingExperience.generated.h"

UCLASS()
class OVERWATCH_API UOWUserFacingExperience : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
	UObject* CreateHostingRequest() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Experience", meta = (AllowedTypes = "Map"))
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Experience", meta = (AllowedTypes = "OWExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
};

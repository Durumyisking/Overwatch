#pragma once

#include "Engine/DataAsset.h"
#include "OWPawnData.generated.h"

class UOWInputConfig;
class UOWCameraMode;

UCLASS()
class OVERWATCH_API UOWPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOWPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Camera")
	TSubclassOf<UOWCameraMode> DefaultCameraMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Input")
	TObjectPtr<UOWInputConfig> InputConfig;
};

#pragma once

#include "Engine/DataTable.h"
#include "GameFramework/GameModeBase.h"
#include "OWGameMode.generated.h"

class UOWExperienceDefinition;
class UOWPawnData;

UCLASS()
class OVERWATCH_API AOWGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AOWGameMode();

	virtual void InitGame(const FString& InMapName, const FString& InOptions, FString& OutErrorMessage) override;
	virtual void InitGameState() override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* InNewPlayer) override;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* InNewPlayer, const FTransform& InSpawnTransform) override;

	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId InExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience);
	const UOWPawnData* GetPawnDataForController(const AController* InController) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OW|Experience", meta = (AllowedTypes = "OWExperienceDefinition"))
	FPrimaryAssetId DefaultExperienceId;

	FPrimaryAssetId RequestedExperienceId;
};

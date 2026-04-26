#pragma once

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
	virtual void InitGameState() final;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) final;
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* InNewPlayer) final;
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* InNewPlayer, const FTransform& InSpawnTransform) final;

	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId InExperienceId);
	bool IsExperienceLoaded() const;
	void OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience);
	const UOWPawnData* GetPawnDataForController(const AController* InController) const;
};

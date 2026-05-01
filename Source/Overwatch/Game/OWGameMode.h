#pragma once

#include "GameFramework/GameModeBase.h"
#include "OWGameMode.generated.h"

class UOWExperienceDefinition;
class UOWPawnData;

/**
 * 서버 전용 매치 오케스트레이터다.
 * Experience 선택, PawnData 조회, 플레이어 시작 흐름만 연결하고 실제 기능 조립은 GameStateComponent에 맡긴다.
 */
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

	/** Controller가 사용할 PawnData를 PlayerState 또는 현재 Experience에서 조회한다. */
	const UOWPawnData* GetPawnDataForController(const AController* InController) const;
};

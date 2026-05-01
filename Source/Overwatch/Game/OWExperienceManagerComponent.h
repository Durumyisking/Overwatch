#pragma once

#include "Components/GameStateComponent.h"
#include "GameFeaturePluginOperationResult.h"
#include "OWExperienceManagerComponent.generated.h"

class UOWExperienceDefinition;

enum class EOWExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	ExecutingActions,
	Loaded,
	Deactivating,
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOWExperienceLoaded, const UOWExperienceDefinition*);
/**
 * GameState가 소유하는 Experience 로딩 상태 관리자다.
 * Experience 에셋 로드, GameFeature 활성화, Action 실행을 순서대로 진행하고 완료 이벤트를 발행한다.
 */
UCLASS()
class OVERWATCH_API UOWExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	bool IsExperienceLoaded() const { return (LoadState == EOWExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

	/** Experience가 이미 로드되었으면 즉시 호출하고, 아직이면 완료 델리게이트에 등록한다. */
	void CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate&& Delegate);
	void ServerSetCurrentExperience(FPrimaryAssetId InExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UOWExperienceDefinition* GetCurrentExperienceChecked() const;

	/** 현재 로드 중이거나 로드 완료된 Experience */
	UPROPERTY()
	TObjectPtr<const UOWExperienceDefinition> CurrentExperience;

	/** Experience의 로딩 상태를 모니터링 */
	EOWExperienceLoadState LoadState = EOWExperienceLoadState::Unloaded;
	/** Experience 로딩이 완료된 이후, Broadcasting Delegate */
	FOnOWExperienceLoaded OnExperienceLoaded;

	/** 활성화된 GameFeature Plugin들 */
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
};

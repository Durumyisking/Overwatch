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
 * HakExperienceManagerComponent
 * - 말 그대로, 해당 component는 game state를 owner로 가지면서, experience의 상태 정보를 가지고 있는 component이다
 * - 뿐만 아니라, manager라는 단어가 포함되어 있듯이, experience 로딩 상태 업데이트 및 이벤트를 관리한다
 */
UCLASS()
class OVERWATCH_API UOWExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	bool IsExperienceLoaded() const { return (LoadState == EOWExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }

/**
 * 아래의 OnExperienceLoaded에 바인딩하거나, 이미 Experience 로딩이 완료되었다면 바로 호출함
 */
	void CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate&& Delegate);
	void ServerSetCurrentExperience(FPrimaryAssetId InExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UOWExperienceDefinition* GetCurrentExperienceChecked() const;

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

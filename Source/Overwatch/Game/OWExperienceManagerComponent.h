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

UCLASS()
class OVERWATCH_API UOWExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	bool IsExperienceLoaded() const { return (LoadState == EOWExperienceLoadState::Loaded) && (CurrentExperience != nullptr); }
	void CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate&& Delegate);
	void ServerSetCurrentExperience(FPrimaryAssetId InExperienceId);
	void StartExperienceLoad();
	void OnExperienceLoadComplete();
	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();
	const UOWExperienceDefinition* GetCurrentExperienceChecked() const;

	UPROPERTY()
	TObjectPtr<const UOWExperienceDefinition> CurrentExperience;

	EOWExperienceLoadState LoadState = EOWExperienceLoadState::Unloaded;
	FOnOWExperienceLoaded OnExperienceLoaded;
	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;
};

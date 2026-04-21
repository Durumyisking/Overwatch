#include "Game/OWExperienceManagerComponent.h"

#include "Core/OWAssetManager.h"
#include "Game/OWExperienceActionSet.h"
#include "Game/OWExperienceDefinition.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesSubsystemSettings.h"

void UOWExperienceManagerComponent::CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate&& Delegate)
{
	if (IsExperienceLoaded())
	{
		Delegate.Execute(CurrentExperience);
		return;
	}

	OnExperienceLoaded.Add(MoveTemp(Delegate));
}

void UOWExperienceManagerComponent::ServerSetCurrentExperience(FPrimaryAssetId InExperienceId)
{
	UOWAssetManager& AssetManager = UOWAssetManager::Get();
	FSoftObjectPath AssetPath = AssetManager.GetPrimaryAssetPath(InExperienceId);
	UClass* AssetClass = Cast<UClass>(AssetPath.TryLoad());
	const UOWExperienceDefinition* Experience = AssetClass ? GetDefault<UOWExperienceDefinition>(AssetClass) : nullptr;

	check(Experience != nullptr);
	check(CurrentExperience == nullptr);

	CurrentExperience = Experience;
	StartExperienceLoad();
}

void UOWExperienceManagerComponent::StartExperienceLoad()
{
	check(CurrentExperience);
	check(LoadState == EOWExperienceLoadState::Unloaded);

	LoadState = EOWExperienceLoadState::Loading;

	UOWAssetManager& AssetManager = UOWAssetManager::Get();
	TSet<FPrimaryAssetId> BundleAssetList;
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());

	TArray<FName> BundlesToLoad;
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();
	if (GIsEditor || (OwnerNetMode != NM_DedicatedServer))
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateClient);
	}

	if (GIsEditor || (OwnerNetMode != NM_Client))
	{
		BundlesToLoad.Add(UGameFeaturesSubsystemSettings::LoadStateServer);
	}

	const FStreamableDelegate OnAssetsLoadedDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::OnExperienceLoadComplete);
	TSharedPtr<FStreamableHandle> Handle = AssetManager.ChangeBundleStateForPrimaryAssets(
		BundleAssetList.Array(),
		BundlesToLoad,
		{},
		false,
		FStreamableDelegate(),
		FStreamableManager::AsyncLoadHighPriority);

	if (!Handle.IsValid() || Handle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadedDelegate);
		return;
	}

	Handle->BindCompleteDelegate(OnAssetsLoadedDelegate);
	Handle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadedDelegate]()
	{
		OnAssetsLoadedDelegate.ExecuteIfBound();
	}));
}

void UOWExperienceManagerComponent::OnExperienceLoadComplete()
{
	check(LoadState == EOWExperienceLoadState::Loading);
	check(CurrentExperience);

	GameFeaturePluginURLs.Reset();

	auto CollectGameFeaturePluginURLs = [this](const TArray<FString>& InPluginNames)
	{
		for (const FString& PluginName : InPluginNames)
		{
			FString PluginURL;
			if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
			{
				GameFeaturePluginURLs.AddUnique(PluginURL);
			}
		}
	};

	CollectGameFeaturePluginURLs(CurrentExperience->GameFeaturesToEnable);

	NumGameFeaturePluginsLoading = GameFeaturePluginURLs.Num();
	if (NumGameFeaturePluginsLoading <= 0)
	{
		OnExperienceFullLoadCompleted();
		return;
	}

	LoadState = EOWExperienceLoadState::LoadingGameFeatures;
	for (const FString& PluginURL : GameFeaturePluginURLs)
	{
		UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &ThisClass::OnGameFeaturePluginLoadComplete));
	}
}

void UOWExperienceManagerComponent::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	NumGameFeaturePluginsLoading--;
	if (NumGameFeaturePluginsLoading == 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void UOWExperienceManagerComponent::OnExperienceFullLoadCompleted()
{
	check(LoadState != EOWExperienceLoadState::Loaded);

	LoadState = EOWExperienceLoadState::ExecutingActions;

	FGameFeatureActivatingContext Context;
	if (const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld()))
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	auto ActivateListOfActions = [&Context](const TArray<UGameFeatureAction*>& InActions)
	{
		for (UGameFeatureAction* Action : InActions)
		{
			if (!Action)
			{
				continue;
			}

			Action->OnGameFeatureRegistering();
			Action->OnGameFeatureLoading();
			Action->OnGameFeatureActivating(Context);
		}
	};

	ActivateListOfActions(CurrentExperience->Actions);

	for (const TObjectPtr<UOWExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet)
		{
			ActivateListOfActions(ActionSet->Actions);
		}
	}

	LoadState = EOWExperienceLoadState::Loaded;
	OnExperienceLoaded.Broadcast(CurrentExperience);
	OnExperienceLoaded.Clear();
}

const UOWExperienceDefinition* UOWExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	check(LoadState == EOWExperienceLoadState::Loaded);
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

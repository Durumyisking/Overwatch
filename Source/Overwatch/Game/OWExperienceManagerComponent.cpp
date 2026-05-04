#include "Game/OWExperienceManagerComponent.h"

#include "Core/OWAssetManager.h"
#include "Game/OWExperienceActionSet.h"
#include "Game/OWExperienceDefinition.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeaturesSubsystemSettings.h"
#include "Net/UnrealNetwork.h"

UOWExperienceManagerComponent::UOWExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

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

	// Experience와 ActionSet은 둘 다 PrimaryDataAsset이므로 Bundle 로딩 대상에 포함한다.
	BundleAssetList.Add(CurrentExperience->GetPrimaryAssetId());
	for (const TObjectPtr<UOWExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet)
		{
			BundleAssetList.Add(ActionSet->GetPrimaryAssetId());
		}
	}

	TArray<FName> BundlesToLoad;
	const ENetMode OwnerNetMode = GetOwner()->GetNetMode();

	// Standalone/Editor에서는 Client, Server 번들을 모두 로드한다.
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
		// 비동기 핸들이 없거나 이미 완료되었으면 완료 delegate를 즉시 실행한다.
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

	// Experience 본체와 ActionSet이 요구하는 GameFeature Plugin을 모두 수집한다.
	CollectGameFeaturePluginURLs(CurrentExperience->GameFeaturesToEnable);
	for (const TObjectPtr<UOWExperienceActionSet>& ActionSet : CurrentExperience->ActionSets)
	{
		if (ActionSet)
		{
			CollectGameFeaturePluginURLs(ActionSet->GameFeaturesToEnable);
		}
	}

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
		// 현재 Experience가 로드된 World에만 GameFeatureAction을 적용한다.
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

			// Experience가 직접 보유한 Action은 GameFeature 플러그인 밖에서 실행되므로 생명주기를 수동으로 호출한다.
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

void UOWExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentExperience);
}

void UOWExperienceManagerComponent::OnRep_CurrentExperience()
{
	StartExperienceLoad();
}

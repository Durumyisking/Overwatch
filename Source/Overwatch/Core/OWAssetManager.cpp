#include "OWAssetManager.h"

#include "OWLog.h"
#include "Types/OWGameplayTags.h"
#include "Engine/Engine.h"
#include "Misc/ScopeLock.h"

void UOWAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FOWGameplayTags::InitializeNativeTags();
}

UOWAssetManager& UOWAssetManager::Get()
{
	UOWAssetManager* AssetManager = Cast<UOWAssetManager>(GEngine ? GEngine->AssetManager : nullptr);
	if (!AssetManager)
	{
		OW_LOG(LogOWCore, Fatal, "AssetManagerClassName must be set to UOWAssetManager.");
	}

	return *AssetManager;
}

UObject* UOWAssetManager::SynchronousLoadAsset(const FSoftObjectPath& InAssetPath)
{
	if (!InAssetPath.IsValid())
	{
		return nullptr;
	}

	if (IsInitialized())
	{
		return GetStreamableManager().LoadSynchronous(InAssetPath);
	}

	return InAssetPath.TryLoad();
}

void UOWAssetManager::AddLoadedAsset(UObject* InAsset)
{
	if (!InAsset)
	{
		return;
	}

	FScopeLock Lock(&SyncObject);
	LoadedAssets.Add(InAsset);
}

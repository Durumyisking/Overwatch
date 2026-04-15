#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "OWAssetManager.generated.h"

UCLASS()
class OVERWATCH_API UOWAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	virtual void StartInitialLoading() override;

	static UOWAssetManager& Get();
	static UObject* SynchronousLoadAsset(const FSoftObjectPath& InAssetPath);

	template <typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& InAssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static AssetType* GetAsset(const FPrimaryAssetId& InPrimaryAssetId, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftObjectPtr<AssetType>& InAssetPointer, bool bKeepInMemory = true);

	template <typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const FPrimaryAssetId& InPrimaryAssetId, bool bKeepInMemory = true);

	void AddLoadedAsset(UObject* InAsset);

private:
	UPROPERTY(Transient)
	TSet<TObjectPtr<UObject>> LoadedAssets;

	FCriticalSection SyncObject;
};

template <typename AssetType>
AssetType* UOWAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& InAssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = InAssetPointer.Get();
	if (LoadedAsset)
	{
		if (bKeepInMemory)
		{
			Get().AddLoadedAsset(LoadedAsset);
		}

		return LoadedAsset;
	}

	const FSoftObjectPath AssetPath = InAssetPointer.ToSoftObjectPath();
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
	if (LoadedAsset && bKeepInMemory)
	{
		Get().AddLoadedAsset(LoadedAsset);
	}

	return LoadedAsset;
}

template <typename AssetType>
AssetType* UOWAssetManager::GetAsset(const FPrimaryAssetId& InPrimaryAssetId, bool bKeepInMemory)
{
	const FSoftObjectPath AssetPath = Get().GetPrimaryAssetPath(InPrimaryAssetId);
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	AssetType* LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
	if (LoadedAsset && bKeepInMemory)
	{
		Get().AddLoadedAsset(LoadedAsset);
	}

	return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UOWAssetManager::GetSubclass(const TSoftObjectPtr<AssetType>& InAssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedClass = InAssetPointer.Get();
	if (LoadedClass)
	{
		if (bKeepInMemory)
		{
			Get().AddLoadedAsset(LoadedClass.Get());
		}

		return LoadedClass;
	}

	const FSoftObjectPath AssetPath = InAssetPointer.ToSoftObjectPath();
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	LoadedClass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
	if (LoadedClass && bKeepInMemory)
	{
		Get().AddLoadedAsset(LoadedClass.Get());
	}

	return LoadedClass;
}

template <typename AssetType>
TSubclassOf<AssetType> UOWAssetManager::GetSubclass(const FPrimaryAssetId& InPrimaryAssetId, bool bKeepInMemory)
{
	const FSoftObjectPath AssetPath = Get().GetPrimaryAssetPath(InPrimaryAssetId);
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}

	TSubclassOf<AssetType> LoadedClass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
	if (LoadedClass && bKeepInMemory)
	{
		Get().AddLoadedAsset(LoadedClass.Get());
	}

	return LoadedClass;
}

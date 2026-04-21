#pragma once

#include "GameFeatures/OWGameFeatureAction_WorldActionBase.h"
#include "Input/OWMappableConfigPair.h"
#include "OWGameFeatureAction_AddInputConfig.generated.h"

struct FComponentRequestHandle;
class APawn;

UCLASS()
class OVERWATCH_API UOWGameFeatureAction_AddInputConfig : public UOWGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext) override;

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	void HandlePawnExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext);
	void AddInputConfig(APawn* InPawn, FPerContextData& InActiveData);
	void RemoveInputConfig(APawn* InPawn, FPerContextData& InActiveData);
	void Reset(FPerContextData& InActiveData);

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	UPROPERTY(EditAnywhere)
	TArray<FOWMappableConfigPair> InputConfigs;
};

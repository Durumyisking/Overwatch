#pragma once

#include "GameFeatures/OWGameFeatureAction_WorldActionBase.h"
#include "OWGameFeatureAction_AddInputBinding.generated.h"

class APawn;
class UOWInputConfig;
struct FComponentRequestHandle;

/**
 * GameFeature가 활성화될 때 준비된 Pawn의 HeroComponent에 InputConfig를 추가 바인딩한다.
 * InputMappingContext 등록은 AddInputContextMapping 액션이 담당한다.
 */
UCLASS(meta = (DisplayName = "Add Input Binding"))
class OVERWATCH_API UOWGameFeatureAction_AddInputBinding : public UOWGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	void HandlePawnExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext);
	void AddInputMappingForPlayer(APawn* InPawn, FPerContextData& InActiveData);
	void RemoveInputMapping(APawn* InPawn, FPerContextData& InActiveData);
	void Reset(FPerContextData& InActiveData);

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
	TArray<TSoftObjectPtr<const UOWInputConfig>> InputConfigs;
};

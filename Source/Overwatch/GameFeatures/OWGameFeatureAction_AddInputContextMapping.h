#pragma once

#include "GameFeatures/OWGameFeatureAction_WorldActionBase.h"
#include "Input/OWInputMappingContextAndPriority.h"
#include "OWGameFeatureAction_AddInputContextMapping.generated.h"

class APlayerController;
class ULocalPlayer;
class UPlayer;
struct FComponentRequestHandle;

/**
 * GameFeature가 활성화될 때 LocalPlayer의 EnhancedInput에 InputMappingContext를 추가한다.
 * 이 액션은 입력 액션을 게임플레이에 바인딩하지 않고, 키/액션 매핑 컨텍스트만 관리한다.
 */
UCLASS(meta = (DisplayName = "Add Input Context Mapping"))
class OVERWATCH_API UOWGameFeatureAction_AddInputContextMapping : public UOWGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	virtual void AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};

	void RegisterInputMappingContexts();
	void RegisterInputContextMappingsForGameInstance(UGameInstance* InGameInstance);
	void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* InLocalPlayer);
	void UnregisterInputMappingContexts();
	void UnregisterInputContextMappingsForGameInstance(UGameInstance* InGameInstance);
	void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* InLocalPlayer);

	void HandleControllerExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext);
	void AddInputMappingForPlayer(UPlayer* InPlayer, FPerContextData& InActiveData);
	void RemoveInputMapping(APlayerController* InPlayerController, FPerContextData& InActiveData);
	void Reset(FPerContextData& InActiveData);

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
	FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;

	UPROPERTY(EditAnywhere, Category = "Input")
	TArray<FOWInputMappingContextAndPriority> InputMappings;
};

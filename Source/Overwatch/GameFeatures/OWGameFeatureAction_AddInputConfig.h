#pragma once

#include "GameFeatures/OWGameFeatureAction_WorldActionBase.h"
#include "Input/OWInputMappingContextAndPriority.h"
#include "OWGameFeatureAction_AddInputConfig.generated.h"

struct FComponentRequestHandle;
class APlayerController;
class ULocalPlayer;


/**
 * GameFeature가 활성화될 때 로컬 플레이어에 입력 매핑 컨텍스트를 추가한다.
 * HeroComponent의 입력 바인딩 가능 이벤트를 기준으로 LocalPlayer 준비 시점을 맞춘다.
 */
UCLASS(meta = (DisplayName = "Add Input Mapping"))
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
		TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
	};

	/** Controller 확장 이벤트를 받아 준비된 LocalPlayer에 입력 매핑을 추가하거나 제거한다. */
	void HandleControllerExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext);

	/** 입력 매핑 컨텍스트를 EnhancedInputLocalPlayerSubsystem에 등록한다. */
	void AddInputMappingForPlayer(ULocalPlayer* InLocalPlayer, APlayerController* InPlayerController, FPerContextData& InActiveData);
	void RemoveInputMapping(APlayerController* InPlayerController, FPerContextData& InActiveData);

	/** 활성화 컨텍스트가 끝날 때 등록한 입력과 확장 요청을 정리한다. */
	void Reset(FPerContextData& InActiveData);

	/** GameFeature 활성화 컨텍스트별 입력 등록 상태 */
	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	/** EnhancedInput에 추가할 InputMappingContext 목록 */
	UPROPERTY(EditAnywhere)
	TArray<FOWInputMappingContextAndPriority> InputMappings;
};

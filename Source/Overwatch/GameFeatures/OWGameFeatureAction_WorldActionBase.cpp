#include "GameFeatures/OWGameFeatureAction_WorldActionBase.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "OWLog.h"

void UOWGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(this, &ThisClass::HandleGameInstanceStart, FGameFeatureStateChangeContext(Context));

	// 이미 초기화된 GameInstance와 연결된 모든 World에 Action을 적용한다.
	int32 AppliedWorldCount = 0;
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, Context);
			++AppliedWorldCount;
		}
	}

	OW_LOG(LogOWGame, Log, TEXT("GameFeatureAction activated. Action=%s Class=%s AppliedWorlds=%d"),
		*GetNameSafe(this),
		*GetNameSafe(GetClass()),
		AppliedWorldCount);
}

void UOWGameFeatureAction_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	if (FDelegateHandle* FoundHandle = GameInstanceStartHandles.Find(Context))
	{
		// GameFeature가 내려갈 때 새 GameInstance 시작 콜백도 같이 해제한다.
		FWorldDelegates::OnStartGameInstance.Remove(*FoundHandle);
		GameInstanceStartHandles.Remove(Context);
	}
}

void UOWGameFeatureAction_WorldActionBase::HandleGameInstanceStart(UGameInstance* InGameInstance, FGameFeatureStateChangeContext InChangeContext)
{
	if (FWorldContext* WorldContext = InGameInstance ? InGameInstance->GetWorldContext() : nullptr)
	{
		// GameFeature 활성화 이후 시작된 GameInstance에도 같은 World 필터를 적용한다.
		if (InChangeContext.ShouldApplyToWorldContext(*WorldContext))
		{
			AddToWorld(*WorldContext, InChangeContext);
		}
	}
}

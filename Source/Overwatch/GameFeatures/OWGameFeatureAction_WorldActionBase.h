#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "OWGameFeatureAction_WorldActionBase.generated.h"

struct FWorldContext;

/** 월드별 실행 로직이 필요한 GameFeatureAction의 공통 기반 클래스다. */
UCLASS(Abstract)
class OVERWATCH_API UOWGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;

	/** 액션별 월드 적용 로직은 파생 클래스가 구현한다. */
	virtual void AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext) PURE_VIRTUAL(UOWGameFeatureAction_WorldActionBase::AddToWorld, );
};

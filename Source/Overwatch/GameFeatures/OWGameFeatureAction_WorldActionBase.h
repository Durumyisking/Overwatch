#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "OWGameFeatureAction_WorldActionBase.generated.h"

struct FWorldContext;

UCLASS(Abstract)
class OVERWATCH_API UOWGameFeatureAction_WorldActionBase : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext) PURE_VIRTUAL(UOWGameFeatureAction_WorldActionBase::AddToWorld, );
};

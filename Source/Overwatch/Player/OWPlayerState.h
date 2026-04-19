#pragma once

#include "GameFramework/PlayerState.h"
#include "OWPlayerState.generated.h"

struct FOWHeroData;
class UOWAbilitySystemComponent;

UCLASS()
class OVERWATCH_API AOWPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AOWPlayerState();
	virtual void PostInitializeComponents() override;

	void SetHeroData(const FOWHeroData* InHeroData);

protected:
	UPROPERTY(VisibleAnywhere, Category = "OW|PlayerState")
	TObjectPtr<UOWAbilitySystemComponent> AbilitySystemComponent;
};

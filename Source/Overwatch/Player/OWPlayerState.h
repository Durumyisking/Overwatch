#pragma once

#include "AbilitySystem/OWAbilitySet.h"
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

	UOWAbilitySystemComponent* GetAbilitySystemComponent() const;
	void SetHeroData(const FOWHeroData* InHeroData);

protected:
	UPROPERTY(VisibleAnywhere, Category = "OW|PlayerState")
	TObjectPtr<UOWAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UOWAbilitySet> AppliedAbilitySet;

	UPROPERTY()
	FOWAbilitySet_GrantedHandles GrantedAbilityHandles;
};

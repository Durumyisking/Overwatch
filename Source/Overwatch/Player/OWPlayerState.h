#pragma once

#include "AbilitySystem/OWAbilitySet.h"
#include "GameFramework/PlayerState.h"
#include "OWPlayerState.generated.h"

struct FOWHeroData;
class UOWAbilitySystemComponent;
class UOWExperienceDefinition;
class UOWPawnData;

UCLASS()
class OVERWATCH_API AOWPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AOWPlayerState();
	virtual void PostInitializeComponents() override;

	UOWAbilitySystemComponent* GetAbilitySystemComponent() const;
	template <class T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	void OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience);
	void SetPawnData(const UOWPawnData* InPawnData);
	void SetHeroData(const FOWHeroData* InHeroData);

protected:
	UPROPERTY(VisibleAnywhere, Category = "OW|PlayerState")
	TObjectPtr<UOWAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UOWAbilitySet> AppliedAbilitySet;

	UPROPERTY()
	FOWAbilitySet_GrantedHandles GrantedAbilityHandles;

	UPROPERTY()
	TObjectPtr<const UOWPawnData> PawnData;
};

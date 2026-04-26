#pragma once

#include "GameFramework/PlayerState.h"
#include "OWPlayerState.generated.h"

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UOWAbilitySystemComponent* GetAbilitySystemComponent() const;
	template <class T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	void OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience);
	void SetPawnData(const UOWPawnData* InPawnData);

protected:
	UPROPERTY(VisibleAnywhere, Category = "OW|PlayerState")
	TObjectPtr<UOWAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Replicated)
	TObjectPtr<const UOWPawnData> PawnData;
};

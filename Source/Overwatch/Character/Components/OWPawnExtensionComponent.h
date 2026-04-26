#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "OWPawnExtensionComponent.generated.h"

class UOWPawnData;

UCLASS()
class OVERWATCH_API UOWPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UOWPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const FName NAME_ActorFeatureName;

	static UOWPawnExtensionComponent* FindPawnExtensionComponent(const AActor* InActor)
	{
		return InActor ? InActor->FindComponentByClass<UOWPawnExtensionComponent>() : nullptr;
	}

	template <class T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	void SetPawnData(const UOWPawnData* InPawnData);
	void SetupPlayerInputComponent();

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) const override;
	virtual void CheckDefaultInitialization() override;

protected:
	UFUNCTION()
	void OnRep_PawnData();

	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "OW|Pawn")
	TObjectPtr<const UOWPawnData> PawnData;
};

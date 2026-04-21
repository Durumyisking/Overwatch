#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "Input/OWMappableConfigPair.h"
#include "OWHeroComponent.generated.h"

struct FInputActionValue;
class UOWCameraMode;

UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UOWHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static const FName NAME_ActorFeatureName;
	static const FName NAME_BindInputsNow;

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) override;
	virtual void CheckDefaultInitialization() override;

	TSubclassOf<UOWCameraMode> DetermineCameraMode() const;
	void InitializePlayerInput(UInputComponent* InPlayerInputComponent);
	void Input_Move(const FInputActionValue& InInputActionValue);
	void Input_LookMouse(const FInputActionValue& InInputActionValue);

	UPROPERTY(EditAnywhere, Category = "OW|Input")
	TArray<FOWMappableConfigPair> DefaultInputConfigs;
};

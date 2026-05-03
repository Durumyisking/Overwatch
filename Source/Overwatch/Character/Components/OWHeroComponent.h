#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "Input/OWInputMappingContextAndPriority.h"
#include "OWHeroComponent.generated.h"

struct FInputActionValue;
class UOWInputConfig;
class UOWCameraMode;

/**
 * 플레이어가 조종하는 Pawn의 입력과 카메라 처리를 조립하는 컴포넌트다.
 * PawnExtensionComponent의 초기화 상태에 맞춰 실행되며, 실제 능력 규칙은 ASC/Ability 쪽에 둔다.
 */
UCLASS(Blueprintable, Meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UOWHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** GameFrameworkComponentManager에 등록되는 이 컴포넌트의 기능 이름 */
	static const FName NAME_ActorFeatureName;

	/** PawnExtension이 입력 바인딩 가능 시점에 보내는 확장 이벤트 이름 */
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
	void AddAdditionalInputConfig(const UOWInputConfig* InInputConfig);
	void RemoveAdditionalInputConfig(const UOWInputConfig* InInputConfig);
	bool IsReadyToBindInputs() const;
	void InitializePlayerInput(UInputComponent* InPlayerInputComponent);
	void Input_Move(const FInputActionValue& InInputActionValue);
	void Input_LookMouse(const FInputActionValue& InInputActionValue);
	void Input_LookStick(const FInputActionValue& InInputActionValue);
	void Input_Crouch(const FInputActionValue& InInputActionValue);
	void Input_AbilityInputTagPressed(FGameplayTag InInputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InInputTag);

	/** 로컬 플레이어 EnhancedInput에 기본으로 등록할 입력 매핑 */
	UPROPERTY(EditAnywhere, Category = "OW|Input")
	TArray<FOWInputMappingContextAndPriority> DefaultInputMappings;

private:
	TMap<const UOWInputConfig*, TArray<uint32>> AdditionalInputConfigBindHandles;
	bool bReadyToBindInputs = false;
};

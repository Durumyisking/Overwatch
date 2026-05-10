#pragma once

#include "Camera/CameraComponent.h"
#include "Camera/OWCameraMode.h"
#include "GameplayTagContainer.h"
#include "OWCameraComponent.generated.h"

class UOWCameraModeStack;

/** 현재 Pawn에 가장 적합한 카메라 모드를 결정하는 델리게이트 */
DECLARE_DELEGATE_RetVal(TSubclassOf<UOWCameraMode>, FOWCameraModeDelegate);

/**
 * 프로젝트의 기본 카메라 컴포넌트다.
 * CameraModeStack을 통해 여러 카메라 모드를 블렌딩하고 최종 View를 만든다.
 */
UCLASS(ClassGroup = (Camera), Meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UOWCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 지정 Actor에 카메라 컴포넌트가 있으면 반환한다. */
	static UOWCameraComponent* FindCameraComponent(const AActor* InActor)
	{
		return InActor ? InActor->FindComponentByClass<UOWCameraComponent>() : nullptr;
	}

	/** 카메라가 따라가거나 바라볼 대상 Actor를 반환한다. */
	AActor* GetTargetActor() const { return GetOwner(); }

	/** 델리게이트로 선택된 카메라 모드를 스택에 반영한다. */
	void UpdateCameraModes();
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

	virtual void OnRegister() override;
	virtual void GetCameraView(float InDeltaTime, FMinimalViewInfo& OutDesiredView) override;

	/** 카메라 모드들을 블렌딩하기 위한 스택 */
	UPROPERTY()
	TObjectPtr<UOWCameraModeStack> CameraModeStack;

	/** 현재 상황에 맞는 최상위 카메라 모드를 조회하는 델리게이트 */
	FOWCameraModeDelegate DetermineCameraModeDelegate;
};

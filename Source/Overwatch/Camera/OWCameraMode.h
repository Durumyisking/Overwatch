#pragma once

#include "GameplayTagContainer.h"
#include "OWCameraMode.generated.h"

class UOWCameraComponent;

/** 카메라 모드가 계산한 위치, 회전, FOV 결과값 */
struct FOWCameraModeView
{
	FOWCameraModeView();

	void Blend(const FOWCameraModeView& InOther, float InOtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

/** 카메라 모드가 스택 안에서 섞일 때 사용할 블렌딩 곡선 */
UENUM(BlueprintType)
enum class EOWCameraModeBlendFunction : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};

/**
 * 카메라 동작 하나를 표현하는 기본 객체다.
 * 모드별 위치/회전 계산은 UpdateView에서 담당하고, 스택은 그 결과를 블렌딩한다.
 */
UCLASS(Abstract, NotBlueprintable)
class OVERWATCH_API UOWCameraMode : public UObject
{
	GENERATED_BODY()

public:
	UOWCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void UpdateCameraMode(float InDeltaTime);
	virtual void UpdateView(float InDeltaTime);
	void UpdateBlending(float InDeltaTime);

	UOWCameraComponent* GetOWCameraComponent() const;
	AActor* GetTargetActor() const;
	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

	FOWCameraModeView View;

	FGameplayTag GetCameraTypeTag() const { return CameraTypeTag; }
	float GetBlendWeight() const { return BlendWeight; }

	/** 이 모드가 목표로 하는 시야각 */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170.0", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	/** 컨트롤 회전 Pitch의 최소 허용값 */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	/** 컨트롤 회전 Pitch의 최대 허용값 */
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	/** 이 모드로 전환될 때 블렌딩에 걸리는 시간 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	/** 블렌딩 진행률과 현재 가중치 */
	float BlendAlpha;
	float BlendWeight;

	/** Ease 계열 블렌딩 함수에서 사용하는 지수 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	/** 이 모드가 사용할 블렌딩 함수 */
	EOWCameraModeBlendFunction BlendFunction;

	/** 조준처럼 gameplay code가 특정 카메라 상태를 태그로 질의할 수 있게 하는 분류 태그 */
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	FGameplayTag CameraTypeTag;
};

/**
 * 활성 카메라 모드들을 위에서 아래로 평가하고 최종 View를 블렌딩한다.
 */
UCLASS()
class OVERWATCH_API UOWCameraModeStack : public UObject
{
	GENERATED_BODY()

public:
	UOWCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UOWCameraMode* GetCameraModeInstance(TSubclassOf<UOWCameraMode>& InCameraModeClass);
	void PushCameraMode(TSubclassOf<UOWCameraMode>& InCameraModeClass);
	void EvaluateStack(float InDeltaTime, FOWCameraModeView& OutCameraModeView);
	void UpdateStack(float InDeltaTime);
	void BlendStack(FOWCameraModeView& OutCameraModeView) const;
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

	/** 재사용을 위해 생성해 둔 카메라 모드 인스턴스 목록 */
	UPROPERTY()
	TArray<TObjectPtr<UOWCameraMode>> CameraModeInstances;

	/** 현재 활성화되어 블렌딩되는 카메라 모드 스택 */
	UPROPERTY()
	TArray<TObjectPtr<UOWCameraMode>> CameraModeStack;
};

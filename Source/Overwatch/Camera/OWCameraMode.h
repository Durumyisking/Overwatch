#pragma once

#include "OWCameraMode.generated.h"

class UOWCameraComponent;

struct FOWCameraModeView
{
	FOWCameraModeView();

	void Blend(const FOWCameraModeView& InOther, float InOtherWeight);

	FVector Location;
	FRotator Rotation;
	FRotator ControlRotation;
	float FieldOfView;
};

UENUM(BlueprintType)
enum class EOWCameraModeBlendFunction : uint8
{
	Linear,
	EaseIn,
	EaseOut,
	EaseInOut,
	COUNT
};

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

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170.0", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	float BlendAlpha;
	float BlendWeight;

	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	EOWCameraModeBlendFunction BlendFunction;
};

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

	UPROPERTY()
	TArray<TObjectPtr<UOWCameraMode>> CameraModeInstances;

	UPROPERTY()
	TArray<TObjectPtr<UOWCameraMode>> CameraModeStack;
};

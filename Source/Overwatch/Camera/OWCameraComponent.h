#pragma once

#include "Camera/CameraComponent.h"
#include "Camera/OWCameraMode.h"
#include "OWCameraComponent.generated.h"

class UOWCameraModeStack;

DECLARE_DELEGATE_RetVal(TSubclassOf<UOWCameraMode>, FOWCameraModeDelegate);

UCLASS(ClassGroup = (Camera), Meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UOWCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	static UOWCameraComponent* FindCameraComponent(const AActor* InActor)
	{
		return InActor ? InActor->FindComponentByClass<UOWCameraComponent>() : nullptr;
	}

	AActor* GetTargetActor() const { return GetOwner(); }
	void UpdateCameraModes();

	virtual void OnRegister() override;
	virtual void GetCameraView(float InDeltaTime, FMinimalViewInfo& OutDesiredView) override;

	UPROPERTY()
	TObjectPtr<UOWCameraModeStack> CameraModeStack;

	FOWCameraModeDelegate DetermineCameraModeDelegate;
};

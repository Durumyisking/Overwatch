#pragma once

#include "Camera/OWCameraMode.h"
#include "OWCameraMode_ThirdPerson.generated.h"

class UCurveVector;

UCLASS(Abstract, Blueprintable)
class OVERWATCH_API UOWCameraMode_ThirdPerson : public UOWCameraMode
{
	GENERATED_BODY()

public:
	UOWCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void UpdateView(float InDeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;
};

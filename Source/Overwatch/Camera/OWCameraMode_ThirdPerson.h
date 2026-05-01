#pragma once

#include "Camera/OWCameraMode.h"
#include "OWCameraMode_ThirdPerson.generated.h"

class UCurveVector;

/**
 * 3인칭 카메라 모드다.
 * Pitch에 따라 커브에서 오프셋을 읽어 Pivot 주변 카메라 위치를 조정한다.
 */
UCLASS(Abstract, Blueprintable)
class OVERWATCH_API UOWCameraMode_ThirdPerson : public UOWCameraMode
{
	GENERATED_BODY()

public:
	UOWCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void UpdateView(float InDeltaTime) override;

	/** 카메라 Pitch별 타겟 오프셋을 정의하는 커브 */
	UPROPERTY(EditDefaultsOnly, Category = "Third Person")
	TObjectPtr<const UCurveVector> TargetOffsetCurve;
};

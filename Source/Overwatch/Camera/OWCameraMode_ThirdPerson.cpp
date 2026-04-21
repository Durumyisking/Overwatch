#include "Camera/OWCameraMode_ThirdPerson.h"

#include "Curves/CurveVector.h"

UOWCameraMode_ThirdPerson::UOWCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOWCameraMode_ThirdPerson::UpdateView(float InDeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;

	if (TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
		View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
	}
}

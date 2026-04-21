#include "Camera/OWCameraMode.h"

#include "Camera/OWCameraComponent.h"
#include "Camera/OWPlayerCameraManager.h"

FOWCameraModeView::FOWCameraModeView()
	: Location(ForceInit)
	, Rotation(ForceInit)
	, ControlRotation(ForceInit)
	, FieldOfView(OW_CAMERA_DEFAULT_FOV)
{
}

void FOWCameraModeView::Blend(const FOWCameraModeView& InOther, float InOtherWeight)
{
	if (InOtherWeight <= 0.0f)
	{
		return;
	}

	if (InOtherWeight >= 1.0f)
	{
		*this = InOther;
		return;
	}

	Location = FMath::Lerp(Location, InOther.Location, InOtherWeight);

	const FRotator DeltaRotation = (InOther.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (InOtherWeight * DeltaRotation);

	const FRotator DeltaControlRotation = (InOther.ControlRotation - ControlRotation).GetNormalized();
	ControlRotation = ControlRotation + (InOtherWeight * DeltaControlRotation);

	FieldOfView = FMath::Lerp(FieldOfView, InOther.FieldOfView, InOtherWeight);
}

UOWCameraMode::UOWCameraMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	FieldOfView = OW_CAMERA_DEFAULT_FOV;
	ViewPitchMin = OW_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = OW_CAMERA_DEFAULT_PITCH_MAX;
	BlendTime = 0.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
	BlendFunction = EOWCameraModeBlendFunction::EaseOut;
	BlendExponent = 4.0f;
}

void UOWCameraMode::UpdateCameraMode(float InDeltaTime)
{
	UpdateView(InDeltaTime);
	UpdateBlending(InDeltaTime);
}

void UOWCameraMode::UpdateView(float InDeltaTime)
{
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

	View.Location = PivotLocation;
	View.Rotation = PivotRotation;
	View.ControlRotation = View.Rotation;
	View.FieldOfView = FieldOfView;
}

void UOWCameraMode::UpdateBlending(float InDeltaTime)
{
	if (BlendTime > 0.0f)
	{
		BlendAlpha += (InDeltaTime / BlendTime);
	}
	else
	{
		BlendAlpha = 1.0f;
	}

	const float Exponent = BlendExponent > 0.0f ? BlendExponent : 1.0f;
	switch (BlendFunction)
	{
	case EOWCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case EOWCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case EOWCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	case EOWCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
		break;
	default:
		checkNoEntry();
		break;
	}
}

UOWCameraComponent* UOWCameraMode::GetOWCameraComponent() const
{
	return CastChecked<UOWCameraComponent>(GetOuter());
}

AActor* UOWCameraMode::GetTargetActor() const
{
	return GetOWCameraComponent()->GetTargetActor();
}

FVector UOWCameraMode::GetPivotLocation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UOWCameraMode::GetPivotRotation() const
{
	const AActor* TargetActor = GetTargetActor();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

UOWCameraModeStack::UOWCameraModeStack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UOWCameraMode* UOWCameraModeStack::GetCameraModeInstance(TSubclassOf<UOWCameraMode>& InCameraModeClass)
{
	check(InCameraModeClass);

	for (UOWCameraMode* CameraMode : CameraModeInstances)
	{
		if (CameraMode && CameraMode->GetClass() == InCameraModeClass)
		{
			return CameraMode;
		}
	}

	UOWCameraMode* NewCameraMode = NewObject<UOWCameraMode>(GetOuter(), InCameraModeClass, NAME_None, RF_NoFlags);
	check(NewCameraMode);

	CameraModeInstances.Add(NewCameraMode);
	return NewCameraMode;
}

void UOWCameraModeStack::PushCameraMode(TSubclassOf<UOWCameraMode>& InCameraModeClass)
{
	if (!InCameraModeClass)
	{
		return;
	}

	UOWCameraMode* CameraMode = GetCameraModeInstance(InCameraModeClass);
	check(CameraMode);

	const int32 StackSize = CameraModeStack.Num();
	if (StackSize > 0 && CameraModeStack[0] == CameraMode)
	{
		return;
	}

	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		if (CameraModeStack[StackIndex] == CameraMode)
		{
			ExistingStackIndex = StackIndex;
			ExistingStackContribution *= CameraMode->BlendWeight;
			break;
		}

		ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->BlendWeight);
	}

	if (ExistingStackIndex != INDEX_NONE)
	{
		CameraModeStack.RemoveAt(ExistingStackIndex);
	}
	else
	{
		ExistingStackContribution = 0.0f;
	}

	const bool bShouldBlend = (CameraMode->BlendTime > 0.0f) && (CameraModeStack.Num() > 0);
	CameraMode->BlendWeight = bShouldBlend ? ExistingStackContribution : 1.0f;
	CameraModeStack.Insert(CameraMode, 0);
	CameraModeStack.Last()->BlendWeight = 1.0f;
}

void UOWCameraModeStack::EvaluateStack(float InDeltaTime, FOWCameraModeView& OutCameraModeView)
{
	UpdateStack(InDeltaTime);
	BlendStack(OutCameraModeView);
}

void UOWCameraModeStack::UpdateStack(float InDeltaTime)
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	int32 RemoveCount = 0;
	int32 RemoveIndex = INDEX_NONE;

	for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
	{
		UOWCameraMode* CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);

		CameraMode->UpdateCameraMode(InDeltaTime);
		if (CameraMode->BlendWeight >= 1.0f)
		{
			RemoveIndex = StackIndex + 1;
			RemoveCount = StackSize - RemoveIndex;
			break;
		}
	}

	if (RemoveCount > 0)
	{
		CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
	}
}

void UOWCameraModeStack::BlendStack(FOWCameraModeView& OutCameraModeView) const
{
	const int32 StackSize = CameraModeStack.Num();
	if (StackSize <= 0)
	{
		return;
	}

	const UOWCameraMode* CameraMode = CameraModeStack[StackSize - 1];
	check(CameraMode);

	OutCameraModeView = CameraMode->View;

	for (int32 StackIndex = StackSize - 2; StackIndex >= 0; --StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		check(CameraMode);
		OutCameraModeView.Blend(CameraMode->View, CameraMode->BlendWeight);
	}
}

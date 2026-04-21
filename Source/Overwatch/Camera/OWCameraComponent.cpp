#include "Camera/OWCameraComponent.h"

UOWCameraComponent::UOWCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, CameraModeStack(nullptr)
{
}

void UOWCameraComponent::OnRegister()
{
	Super::OnRegister();

	if (!CameraModeStack)
	{
		CameraModeStack = NewObject<UOWCameraModeStack>(this);
	}
}

void UOWCameraComponent::GetCameraView(float InDeltaTime, FMinimalViewInfo& OutDesiredView)
{
	check(CameraModeStack);

	UpdateCameraModes();

	FOWCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(InDeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PlayerController = TargetPawn->GetController<APlayerController>())
		{
			PlayerController->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);
	FieldOfView = CameraModeView.FieldOfView;

	OutDesiredView.Location = CameraModeView.Location;
	OutDesiredView.Rotation = CameraModeView.Rotation;
	OutDesiredView.FOV = CameraModeView.FieldOfView;
	OutDesiredView.OrthoWidth = OrthoWidth;
	OutDesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	OutDesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	OutDesiredView.AspectRatio = AspectRatio;
	OutDesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	OutDesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	OutDesiredView.ProjectionMode = ProjectionMode;
	OutDesiredView.PostProcessBlendWeight = PostProcessBlendWeight;

	if (PostProcessBlendWeight > 0.0f)
	{
		OutDesiredView.PostProcessSettings = PostProcessSettings;
	}
}

void UOWCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);

	if (DetermineCameraModeDelegate.IsBound())
	{
		if (TSubclassOf<UOWCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
}

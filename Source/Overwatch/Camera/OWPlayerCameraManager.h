#pragma once

#include "Camera/PlayerCameraManager.h"
#include "OWPlayerCameraManager.generated.h"

#define OW_CAMERA_DEFAULT_FOV (80.0f)
#define OW_CAMERA_DEFAULT_PITCH_MIN (-89.0f)
#define OW_CAMERA_DEFAULT_PITCH_MAX (89.0f)

UCLASS()
class OVERWATCH_API AOWPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	AOWPlayerCameraManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

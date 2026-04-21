#include "Camera/OWPlayerCameraManager.h"

AOWPlayerCameraManager::AOWPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = OW_CAMERA_DEFAULT_FOV;
	ViewPitchMin = OW_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = OW_CAMERA_DEFAULT_PITCH_MAX;
}

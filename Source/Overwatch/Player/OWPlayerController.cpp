#include "OWPlayerController.h"

#include "Camera/OWPlayerCameraManager.h"

AOWPlayerController::AOWPlayerController()
{
	PlayerCameraManagerClass = AOWPlayerCameraManager::StaticClass();
}

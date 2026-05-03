#include "OWPlayerController.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "Camera/OWPlayerCameraManager.h"
#include "Player/OWPlayerState.h"

AOWPlayerController::AOWPlayerController()
{
	PlayerCameraManagerClass = AOWPlayerCameraManager::StaticClass();
}

void AOWPlayerController::PostProcessInput(const float InDeltaTime, const bool bInGamePaused)
{
	if (UOWAbilitySystemComponent* AbilitySystemComponent = GetOWAbilitySystemComponent())
	{
		// UPlayerInput::ProcessInputStack 이후 모아둔 Ability 입력 큐를 ASC에서 처리한다.
		AbilitySystemComponent->ProcessAbilityInput(InDeltaTime, bInGamePaused);
	}

	Super::PostProcessInput(InDeltaTime, bInGamePaused);
}

AOWPlayerState* AOWPlayerController::GetOWPlayerState() const
{
	return CastChecked<AOWPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UOWAbilitySystemComponent* AOWPlayerController::GetOWAbilitySystemComponent() const
{
	const AOWPlayerState* OWPlayerState = GetOWPlayerState();
	return OWPlayerState ? OWPlayerState->GetOWAbilitySystemComponent() : nullptr;
}

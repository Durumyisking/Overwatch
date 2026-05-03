#pragma once

#include "GameFramework/PlayerController.h"
#include "OWPlayerController.generated.h"

class AOWPlayerState;
class UOWAbilitySystemComponent;

UCLASS()
class OVERWATCH_API AOWPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOWPlayerController();

	virtual void PostProcessInput(const float InDeltaTime, const bool bInGamePaused) override;

	AOWPlayerState* GetOWPlayerState() const;
	UOWAbilitySystemComponent* GetOWAbilitySystemComponent() const;
};

#pragma once

#include "ModularPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "OWPlayerController.generated.h"

class AOWPlayerState;
class UOWAbilitySystemComponent;

UCLASS()
class OVERWATCH_API AOWPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	AOWPlayerController();

	virtual void PostProcessInput(const float InDeltaTime, const bool bInGamePaused) override;

	AOWPlayerState* GetOWPlayerState() const;
	UOWAbilitySystemComponent* GetOWAbilitySystemComponent() const;
};

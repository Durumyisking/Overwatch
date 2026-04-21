#pragma once

#include "GameFramework/GameStateBase.h"
#include "OWGameState.generated.h"

class UOWExperienceManagerComponent;

UCLASS()
class OVERWATCH_API AOWGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AOWGameState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OW|Experience")
	TObjectPtr<UOWExperienceManagerComponent> ExperienceManagerComponent;
};

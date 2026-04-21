#include "OWGameState.h"

#include "Game/OWExperienceManagerComponent.h"

AOWGameState::AOWGameState()
{
	ExperienceManagerComponent = CreateDefaultSubobject<UOWExperienceManagerComponent>(TEXT("ExperienceManagerComponent"));
}

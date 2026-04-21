#include "Game/OWGameInstance.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Core/Types/OWGameplayTags.h"

void UOWGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);
	if (!ensure(ComponentManager))
	{
		return;
	}

	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	ComponentManager->RegisterInitState(GameplayTags.InitState_Spawned, false, FGameplayTag());
	ComponentManager->RegisterInitState(GameplayTags.InitState_DataAvailable, false, GameplayTags.InitState_Spawned);
	ComponentManager->RegisterInitState(GameplayTags.InitState_DataInitialized, false, GameplayTags.InitState_DataAvailable);
	ComponentManager->RegisterInitState(GameplayTags.InitState_GameplayReady, false, GameplayTags.InitState_DataInitialized);
}

void UOWGameInstance::Shutdown()
{
	Super::Shutdown();
}

#include "GameFeatures/OWGameFeatureAction_AddInputConfig.h"

#include "Character/Components/OWHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void UOWGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UOWGameFeatureAction_AddInputConfig::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	if (FPerContextData* ActiveData = ContextData.Find(Context))
	{
		Reset(*ActiveData);
	}
}

void UOWGameFeatureAction_AddInputConfig::AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext)
{
	UWorld* World = InWorldContext.World();
	UGameInstance* GameInstance = InWorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddConfigDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleControllerExtension, InChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddConfigDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UOWGameFeatureAction_AddInputConfig::Reset(FPerContextData& InActiveData)
{
	InActiveData.ExtensionRequestHandles.Empty();

	while (!InActiveData.ControllersAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APlayerController> ControllerPtr = InActiveData.ControllersAddedTo.Top();
		if (ControllerPtr.IsValid())
		{
			RemoveInputMapping(ControllerPtr.Get(), InActiveData);
		}
		else
		{
			InActiveData.ControllersAddedTo.Pop();
		}
	}
}

void UOWGameFeatureAction_AddInputConfig::AddInputMappingForPlayer(ULocalPlayer* InLocalPlayer, APlayerController* InPlayerController, FPerContextData& InActiveData)
{
	if (!InLocalPlayer || !InPlayerController)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return;
	}

	FModifyContextOptions Options;
	Options.bIgnoreAllPressedKeysUntilRelease = false;

	for (const FOWInputMappingContextAndPriority& Mapping : InputMappings)
	{
		if (UInputMappingContext* InputMapping = Mapping.InputMapping.LoadSynchronous())
		{
			if (Mapping.bRegisterWithSettings)
			{
				if (UEnhancedInputUserSettings* Settings = InputSubsystem->GetUserSettings())
				{
					Settings->RegisterInputMappingContext(InputMapping);
				}
			}

			InputSubsystem->AddMappingContext(InputMapping, Mapping.Priority, Options);
		}
	}

	InActiveData.ControllersAddedTo.AddUnique(InPlayerController);
}

void UOWGameFeatureAction_AddInputConfig::RemoveInputMapping(APlayerController* InPlayerController, FPerContextData& InActiveData)
{
	if (ULocalPlayer* LocalPlayer = InPlayerController ? InPlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const FOWInputMappingContextAndPriority& Mapping : InputMappings)
			{
				if (const UInputMappingContext* InputMapping = Mapping.InputMapping.Get())
				{
					InputSubsystem->RemoveMappingContext(InputMapping);

					if (Mapping.bRegisterWithSettings)
					{
						if (UEnhancedInputUserSettings* Settings = InputSubsystem->GetUserSettings())
						{
							Settings->UnregisterInputMappingContext(InputMapping);
						}
					}
				}
			}
		}
	}

	InActiveData.ControllersAddedTo.Remove(InPlayerController);
}

void UOWGameFeatureAction_AddInputConfig::HandleControllerExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext)
{
	APlayerController* PlayerController = CastChecked<APlayerController>(InActor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if (InEventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || InEventName == UOWHeroComponent::NAME_BindInputsNow)
	{
		AddInputMappingForPlayer(PlayerController->GetLocalPlayer(), PlayerController, ActiveData);
	}
	else if (InEventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || InEventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveInputMapping(PlayerController, ActiveData);
	}
}

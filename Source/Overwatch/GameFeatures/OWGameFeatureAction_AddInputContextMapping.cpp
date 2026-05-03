#include "GameFeatures/OWGameFeatureAction_AddInputContextMapping.h"

#include "Character/Components/OWHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "OWGameFeatures"

void UOWGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	RegisterInputMappingContexts();
}

void UOWGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	// UGameFeatureAction_WorldActionBase를 호출하면서 AddToWorld()가 호출된다.
	Super::OnGameFeatureActivating(Context);
}

void UOWGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	if (FPerContextData* ActiveData = ContextData.Find(Context))
	{
		Reset(*ActiveData);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();

	UnregisterInputMappingContexts();
}

#if WITH_EDITOR
EDataValidationResult UOWGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;
	for (const FOWInputMappingContextAndPriority& Mapping : InputMappings)
	{
		if (Mapping.InputMapping.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}

		++Index;
	}

	return Result;
}
#endif

void UOWGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()
{
	RegisterInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this, &ThisClass::RegisterInputContextMappingsForGameInstance);

	// 이미 초기화된 GameInstance에도 등록을 적용한다.
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator(); WorldContextIterator; ++WorldContextIterator)
	{
		RegisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* InGameInstance)
{
	if (!InGameInstance || InGameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
	{
		return;
	}

	InGameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &ThisClass::RegisterInputMappingContextsForLocalPlayer);
	InGameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &ThisClass::UnregisterInputMappingContextsForLocalPlayer);

	for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = InGameInstance->GetLocalPlayerIterator(); LocalPlayerIterator; ++LocalPlayerIterator)
	{
		RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* InLocalPlayer)
{
	if (!ensure(InLocalPlayer))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputUserSettings* Settings = InputSubsystem ? InputSubsystem->GetUserSettings() : nullptr;
	if (!Settings)
	{
		return;
	}

	for (const FOWInputMappingContextAndPriority& Mapping : InputMappings)
	{
		if (!Mapping.bRegisterWithSettings)
		{
			continue;
		}

		if (UInputMappingContext* InputMapping = Mapping.InputMapping.LoadSynchronous())
		{
			Settings->RegisterInputMappingContext(InputMapping);
		}
	}
}

void UOWGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()
{
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);
	RegisterInputContextMappingsForGameInstanceHandle.Reset();

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator(); WorldContextIterator; ++WorldContextIterator)
	{
		UnregisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(UGameInstance* InGameInstance)
{
	if (!InGameInstance)
	{
		return;
	}

	InGameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
	InGameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

	for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = InGameInstance->GetLocalPlayerIterator(); LocalPlayerIterator; ++LocalPlayerIterator)
	{
		UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* InLocalPlayer)
{
	if (!ensure(InLocalPlayer))
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	UEnhancedInputUserSettings* Settings = InputSubsystem ? InputSubsystem->GetUserSettings() : nullptr;
	if (!Settings)
	{
		return;
	}

	for (const FOWInputMappingContextAndPriority& Mapping : InputMappings)
	{
		if (!Mapping.bRegisterWithSettings)
		{
			continue;
		}

		if (const UInputMappingContext* InputMapping = Mapping.InputMapping.Get())
		{
			Settings->UnregisterInputMappingContext(InputMapping);
		}
	}
}

void UOWGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext)
{
	UWorld* World = InWorldContext.World();
	UGameInstance* GameInstance = InWorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		// Controller 확장 이벤트는 LocalPlayer의 EnhancedInputSubsystem이 준비되는 시점을 알려준다.
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddMappingDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleControllerExtension, InChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddMappingDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UOWGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& InActiveData)
{
	InActiveData.ExtensionRequestHandles.Empty();

	// 이 액션이 추가한 MappingContext만 제거한다.
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

void UOWGameFeatureAction_AddInputContextMapping::HandleControllerExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext)
{
	APlayerController* PlayerController = CastChecked<APlayerController>(InActor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if ((InEventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (InEventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(PlayerController, ActiveData);
	}
	else if ((InEventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (InEventName == UOWHeroComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(PlayerController->GetLocalPlayer(), ActiveData);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* InPlayer, FPerContextData& InActiveData)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(InPlayer);
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
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
			InputSubsystem->AddMappingContext(InputMapping, Mapping.Priority, Options);
		}
	}

	if (APlayerController* PlayerController = LocalPlayer->GetPlayerController(nullptr))
	{
		InActiveData.ControllersAddedTo.AddUnique(PlayerController);
	}
}

void UOWGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* InPlayerController, FPerContextData& InActiveData)
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
				}
			}
		}
	}

	InActiveData.ControllersAddedTo.Remove(InPlayerController);
}

#undef LOCTEXT_NAMESPACE

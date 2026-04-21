#include "GameFeatures/OWGameFeatureAction_AddInputConfig.h"

#include "Character/Components/OWHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"

void UOWGameFeatureAction_AddInputConfig::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.PawnsAddedTo.IsEmpty()))
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
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, InChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddConfigDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UOWGameFeatureAction_AddInputConfig::Reset(FPerContextData& InActiveData)
{
	InActiveData.ExtensionRequestHandles.Empty();

	while (!InActiveData.PawnsAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APawn> PawnPtr = InActiveData.PawnsAddedTo.Top();
		if (PawnPtr.IsValid())
		{
			RemoveInputConfig(PawnPtr.Get(), InActiveData);
		}
		else
		{
			InActiveData.PawnsAddedTo.Pop();
		}
	}
}

void UOWGameFeatureAction_AddInputConfig::AddInputConfig(APawn* InPawn, FPerContextData& InActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(InPawn->GetController());
	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			FModifyContextOptions Options;
			Options.bIgnoreAllPressedKeysUntilRelease = false;

			for (const FOWMappableConfigPair& Pair : InputConfigs)
			{
				if (!Pair.bShouldActivateAutomatically)
				{
					continue;
				}

				if (const UPlayerMappableInputConfig* PlayerMappableConfig = Pair.Config.LoadSynchronous())
				{
					for (const TPair<TObjectPtr<UInputMappingContext>, int32>& MappingPair : PlayerMappableConfig->GetMappingContexts())
					{
						if (MappingPair.Key)
						{
							InputSubsystem->AddMappingContext(MappingPair.Key, MappingPair.Value, Options);
						}
					}
				}
			}

			InActiveData.PawnsAddedTo.AddUnique(InPawn);
		}
	}
}

void UOWGameFeatureAction_AddInputConfig::RemoveInputConfig(APawn* InPawn, FPerContextData& InActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(InPawn->GetController());
	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const FOWMappableConfigPair& Pair : InputConfigs)
			{
				if (const UPlayerMappableInputConfig* PlayerMappableConfig = Pair.Config.LoadSynchronous())
				{
					for (const TPair<TObjectPtr<UInputMappingContext>, int32>& MappingPair : PlayerMappableConfig->GetMappingContexts())
					{
						if (MappingPair.Key)
						{
							InputSubsystem->RemoveMappingContext(MappingPair.Key);
						}
					}
				}
			}

			InActiveData.PawnsAddedTo.Remove(InPawn);
		}
	}
}

void UOWGameFeatureAction_AddInputConfig::HandlePawnExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext)
{
	APawn* Pawn = CastChecked<APawn>(InActor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if (InEventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || InEventName == UOWHeroComponent::NAME_BindInputsNow)
	{
		AddInputConfig(Pawn, ActiveData);
	}
	else if (InEventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved)
	{
		RemoveInputConfig(Pawn, ActiveData);
	}
}

#include "GameFeatures/OWGameFeatureAction_AddInputBinding.h"

#include "Character/Components/OWHeroComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Input/OWInputConfig.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#define LOCTEXT_NAMESPACE "OWGameFeatures"

void UOWGameFeatureAction_AddInputBinding::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.PawnsAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}

	Super::OnGameFeatureActivating(Context);
}

void UOWGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	if (FPerContextData* ActiveData = ContextData.Find(Context))
	{
		Reset(*ActiveData);
	}
}

#if WITH_EDITOR
EDataValidationResult UOWGameFeatureAction_AddInputBinding::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;
	for (const TSoftObjectPtr<const UOWInputConfig>& InputConfig : InputConfigs)
	{
		if (InputConfig.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputConfig", "Null InputConfig at index {0}."), Index));
		}

		++Index;
	}

	return Result;
}
#endif

void UOWGameFeatureAction_AddInputBinding::AddToWorld(const FWorldContext& InWorldContext, const FGameFeatureStateChangeContext& InChangeContext)
{
	UWorld* World = InWorldContext.World();
	UGameInstance* GameInstance = InWorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if (GameInstance && World && World->IsGameWorld())
	{
		// Pawn 확장 이벤트는 HeroComponent가 InputComponent를 바인딩할 준비가 되었는지 알려준다.
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddBindingDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, InChangeContext);

			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddBindingDelegate);
			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UOWGameFeatureAction_AddInputBinding::Reset(FPerContextData& InActiveData)
{
	InActiveData.ExtensionRequestHandles.Empty();

	while (!InActiveData.PawnsAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APawn> PawnPtr = InActiveData.PawnsAddedTo.Top();
		if (PawnPtr.IsValid())
		{
			RemoveInputMapping(PawnPtr.Get(), InActiveData);
		}
		else
		{
			InActiveData.PawnsAddedTo.Pop();
		}
	}
}

void UOWGameFeatureAction_AddInputBinding::HandlePawnExtension(AActor* InActor, FName InEventName, FGameFeatureStateChangeContext InChangeContext)
{
	APawn* Pawn = CastChecked<APawn>(InActor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(InChangeContext);

	if ((InEventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (InEventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(Pawn, ActiveData);
	}
	else if ((InEventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (InEventName == UOWHeroComponent::NAME_BindInputsNow))
	{
		AddInputMappingForPlayer(Pawn, ActiveData);
	}
}

void UOWGameFeatureAction_AddInputBinding::AddInputMappingForPlayer(APawn* InPawn, FPerContextData& InActiveData)
{
	APlayerController* PlayerController = InPawn ? Cast<APlayerController>(InPawn->GetController()) : nullptr;
	ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr;
	if (!LocalPlayer)
	{
		return;
	}

	if (!LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		return;
	}

	UOWHeroComponent* HeroComponent = InPawn ? InPawn->FindComponentByClass<UOWHeroComponent>() : nullptr;
	if (!HeroComponent || !HeroComponent->IsReadyToBindInputs())
	{
		return;
	}

	for (const TSoftObjectPtr<const UOWInputConfig>& InputConfigPtr : InputConfigs)
	{
		if (const UOWInputConfig* InputConfig = InputConfigPtr.LoadSynchronous())
		{
			HeroComponent->AddAdditionalInputConfig(InputConfig);
		}
	}

	InActiveData.PawnsAddedTo.AddUnique(InPawn);
}

void UOWGameFeatureAction_AddInputBinding::RemoveInputMapping(APawn* InPawn, FPerContextData& InActiveData)
{
	if (UOWHeroComponent* HeroComponent = InPawn ? InPawn->FindComponentByClass<UOWHeroComponent>() : nullptr)
	{
		for (const TSoftObjectPtr<const UOWInputConfig>& InputConfigPtr : InputConfigs)
		{
			if (const UOWInputConfig* InputConfig = InputConfigPtr.Get())
			{
				HeroComponent->RemoveAdditionalInputConfig(InputConfig);
			}
		}
	}

	InActiveData.PawnsAddedTo.Remove(InPawn);
}

#undef LOCTEXT_NAMESPACE

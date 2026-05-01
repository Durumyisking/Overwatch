#include "Character/Components/OWHeroComponent.h"

#include "Camera/OWCameraComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Character/Components/OWPawnExtensionComponent.h"
#include "Core/Types/OWGameplayTags.h"
#include "Data/OWPawnData.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputActionValue.h"
#include "Input/OWInputComponent.h"
#include "Input/OWInputConfig.h"
#include "Player/OWPlayerState.h"
#include "UserSettings/EnhancedInputUserSettings.h"

const FName UOWHeroComponent::NAME_ActorFeatureName(TEXT("Hero"));
const FName UOWHeroComponent::NAME_BindInputsNow(TEXT("BindInputsNow"));

UOWHeroComponent::UOWHeroComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UOWHeroComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		return;
	}

	RegisterInitStateFeature();
}

void UOWHeroComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(UOWPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	ensure(TryToChangeInitState(FOWGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UOWHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UOWHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	if (Params.FeatureName == UOWPawnExtensionComponent::NAME_ActorFeatureName && Params.FeatureState == GameplayTags.InitState_DataInitialized)
	{
		CheckDefaultInitialization();
	}
}

bool UOWHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) const
{
	check(InManager);

	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();
	AOWPlayerState* PlayerState = GetPlayerState<AOWPlayerState>();

	if (!InCurrentState.IsValid() && InDesiredState == GameplayTags.InitState_Spawned)
	{
		return Pawn != nullptr;
	}

	if (InCurrentState == GameplayTags.InitState_Spawned && InDesiredState == GameplayTags.InitState_DataAvailable)
	{
		return PlayerState != nullptr;
	}

	if (InCurrentState == GameplayTags.InitState_DataAvailable && InDesiredState == GameplayTags.InitState_DataInitialized)
	{
		return PlayerState && InManager->HasFeatureReachedInitState(Pawn, UOWPawnExtensionComponent::NAME_ActorFeatureName, GameplayTags.InitState_DataInitialized);
	}

	if (InCurrentState == GameplayTags.InitState_DataInitialized && InDesiredState == GameplayTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UOWHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState)
{
	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	if (InCurrentState != GameplayTags.InitState_DataAvailable || InDesiredState != GameplayTags.InitState_DataInitialized)
	{
		return;
	}

	APawn* Pawn = GetPawn<APawn>();
	if (!ensure(Pawn))
	{
		return;
	}

	const UOWPawnData* PawnData = nullptr;
	if (UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		PawnData = PawnExtensionComponent->GetPawnData<UOWPawnData>();
	}

	if (Pawn->IsLocallyControlled() && PawnData)
	{
		if (UOWCameraComponent* CameraComponent = UOWCameraComponent::FindCameraComponent(Pawn))
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}

		if (Pawn->InputComponent)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
	}
}

void UOWHeroComponent::CheckDefaultInitialization()
{
	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { GameplayTags.InitState_Spawned, GameplayTags.InitState_DataAvailable, GameplayTags.InitState_DataInitialized, GameplayTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

TSubclassOf<UOWCameraMode> UOWHeroComponent::DetermineCameraMode() const
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return nullptr;
	}

	if (UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UOWPawnData* PawnData = PawnExtensionComponent->GetPawnData<UOWPawnData>())
		{
			return PawnData->DefaultCameraMode;
		}
	}

	return nullptr;
}

void UOWHeroComponent::InitializePlayerInput(UInputComponent* InPlayerInputComponent)
{
	check(InPlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return;
	}

	InputSubsystem->ClearAllMappings();

	if (const UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UOWPawnData* PawnData = PawnExtensionComponent->GetPawnData<UOWPawnData>())
		{
			if (const UOWInputConfig* InputConfig = PawnData->InputConfig)
			{
				FModifyContextOptions Options;
				Options.bIgnoreAllPressedKeysUntilRelease = false;

				for (const FOWInputMappingContextAndPriority& Mapping : DefaultInputMappings)
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

				UOWInputComponent* OWInputComponent = CastChecked<UOWInputComponent>(InPlayerInputComponent);
				OWInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::Get().GetInputTagMove(), ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
				OWInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::Get().GetInputTagLookMouse(), ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
			}
		}
	}

	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PlayerController), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UOWHeroComponent::Input_Move(const FInputActionValue& InInputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	if (!Controller)
	{
		return;
	}

	const FVector2D Value = InInputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UOWHeroComponent::Input_LookMouse(const FInputActionValue& InInputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InInputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(-Value.Y);
	}
}

#include "Character/Components/OWHeroComponent.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
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
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "OWLog.h"
#include "Player/OWPlayerState.h"
#include "UserSettings/EnhancedInputUserSettings.h"

namespace
{
constexpr float LookYawRate = 300.0f;
constexpr float LookPitchRate = 165.0f;
}

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

	// PawnExtensionComponent의 InitState 변화를 관찰한다.
	BindOnActorInitStateChanged(UOWPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

	// Spawned 상태로 진입한 뒤, 나머지 기본 초기화를 이어서 시도한다.
	ensure(TryToChangeInitState(FOWGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UOWHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	AdditionalInputConfigBindHandles.Empty();
	bReadyToBindInputs = false;

	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UOWHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	if (Params.FeatureName == UOWPawnExtensionComponent::NAME_ActorFeatureName && Params.FeatureState == GameplayTags.InitState_DataInitialized)
	{
		// PawnExtension이 다른 Feature 초기화를 끝냈다고 알리면 Hero도 다음 상태로 진행한다.
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
		// 실제 Pawn이 있으면 Spawned 상태로 전환할 수 있다.
		return Pawn != nullptr;
	}

	if (InCurrentState == GameplayTags.InitState_Spawned && InDesiredState == GameplayTags.InitState_DataAvailable)
	{
		// PlayerState는 리스폰을 넘어 유지되는 플레이어 데이터의 소유자이므로 반드시 필요하다.
		if (!Pawn || !PlayerState)
		{
			return false;
		}

		// Authority 또는 Autonomous Pawn은 PlayerState 소유권이 Controller에 등록될 때까지 기다린다.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();
			const bool bHasControllerPairedWithPlayerState = Controller && Controller->PlayerState && Controller->PlayerState->GetOwner() == Controller;
			if (!bHasControllerPairedWithPlayerState)
			{
				return false;
			}
		}

		if (Pawn->IsLocallyControlled())
		{
			APlayerController* PlayerController = GetController<APlayerController>();

			// 로컬 제어 Pawn은 입력 컴포넌트와 LocalPlayer가 준비되어야 한다.
			if (!Pawn->InputComponent || !PlayerController || !PlayerController->GetLocalPlayer())
			{
				return false;
			}
		}

		return true;
	}

	if (InCurrentState == GameplayTags.InitState_DataAvailable && InDesiredState == GameplayTags.InitState_DataInitialized)
	{
		// PlayerState와 PawnExtension의 DataInitialized 상태를 기다린다.
		return PlayerState && InManager->HasFeatureReachedInitState(Pawn, UOWPawnExtensionComponent::NAME_ActorFeatureName, GameplayTags.InitState_DataInitialized);
	}

	if (InCurrentState == GameplayTags.InitState_DataInitialized && InDesiredState == GameplayTags.InitState_GameplayReady)
	{
		// 이후 Ability 초기화 검증이 필요해지면 이 단계에서 조건을 추가한다.
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

	// DataAvailable -> DataInitialized 단계에서 Pawn은 Controller에 possess되어 준비된 상태다.
	APawn* Pawn = GetPawn<APawn>();
	if (!ensure(Pawn))
	{
		return;
	}

	AOWPlayerState* PlayerState = GetPlayerState<AOWPlayerState>();
	if (!ensure(PlayerState))
	{
		return;
	}

	const UOWPawnData* PawnData = nullptr;
	if (UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		PawnData = PawnExtensionComponent->GetPawnData<UOWPawnData>();

		// PlayerState는 지속 데이터를 소유한다. ASC도 PlayerState에 있으므로 PawnExtension을 통해 현재 Pawn을 AvatarActor로 연결한다.
		PawnExtensionComponent->InitializeAbilitySystem(PlayerState->GetOWAbilitySystemComponent(), PlayerState);
	}

	if (Pawn->IsLocallyControlled() && PawnData)
	{
		OW_LOG(LogOWGame, Log, TEXT("Hero input init gate passed. Pawn=%s Controller=%s PlayerState=%s PawnData=%s InputComponent=%s InputConfig=%s DefaultMappings=%d"),
			*GetNameSafe(Pawn),
			*GetNameSafe(Pawn->GetController()),
			*GetNameSafe(PlayerState),
			*GetNameSafe(PawnData),
			*GetNameSafe(Pawn->InputComponent),
			*GetNameSafe(PawnData->InputConfig),
			DefaultInputMappings.Num());

		// 현재 Pawn에 붙은 CameraComponent를 찾아 PawnData의 기본 CameraMode를 결정하도록 delegate를 연결한다.
		if (UOWCameraComponent* CameraComponent = UOWCameraComponent::FindCameraComponent(Pawn))
		{
			CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
		}

		if (Pawn->InputComponent)
		{
			InitializePlayerInput(Pawn->InputComponent);
		}
		else
		{
			OW_WARN(LogOWGame, TEXT("Hero input init skipped because Pawn InputComponent is null. Pawn=%s"), *GetNameSafe(Pawn));
		}
	}
	else
	{
		OW_LOG(LogOWGame, Log, TEXT("Hero input init gate blocked. Pawn=%s bLocallyControlled=%s PawnData=%s"),
			*GetNameSafe(Pawn),
			Pawn && Pawn->IsLocallyControlled() ? TEXT("true") : TEXT("false"),
			*GetNameSafe(PawnData));
	}
}

void UOWHeroComponent::CheckDefaultInitialization()
{
	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { GameplayTags.InitState_Spawned, GameplayTags.InitState_DataAvailable, GameplayTags.InitState_DataInitialized, GameplayTags.InitState_GameplayReady };

	// BeginPlay에서 Spawned를 찍은 뒤 DataAvailable -> DataInitialized -> GameplayReady 순서로 가능한 만큼 진행한다.
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

	OW_LOG(LogOWGame, Log, TEXT("InitializePlayerInput entered. InputComponent=%s Class=%s"),
		*GetNameSafe(InPlayerInputComponent),
		*GetNameSafe(InPlayerInputComponent->GetClass()));

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		OW_WARN(LogOWGame, TEXT("InitializePlayerInput aborted: Pawn is null."));
		return;
	}

	const APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController)
	{
		OW_WARN(LogOWGame, TEXT("InitializePlayerInput aborted: PlayerController is null. Pawn=%s Controller=%s"),
			*GetNameSafe(Pawn),
			*GetNameSafe(Pawn->GetController()));
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		OW_WARN(LogOWGame, TEXT("InitializePlayerInput aborted: LocalPlayer is null. Pawn=%s PlayerController=%s"),
			*GetNameSafe(Pawn),
			*GetNameSafe(PlayerController));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		OW_WARN(LogOWGame, TEXT("InitializePlayerInput aborted: EnhancedInputLocalPlayerSubsystem is null. LocalPlayer=%s"),
			*GetNameSafe(LocalPlayer));
		return;
	}

	// EnhancedInputLocalPlayerSubsystem의 기존 MappingContext를 비우고 PawnData 기준으로 다시 구성한다.
	InputSubsystem->ClearAllMappings();
	OW_LOG(LogOWGame, Log, TEXT("InitializePlayerInput cleared mappings. Pawn=%s PlayerController=%s DefaultMappings=%d"),
		*GetNameSafe(Pawn),
		*GetNameSafe(PlayerController),
		DefaultInputMappings.Num());

	// PawnExtensionComponent -> PawnData -> InputConfig 순서로 입력 설정을 찾는다.
	if (const UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	{
		if (const UOWPawnData* PawnData = PawnExtensionComponent->GetPawnData<UOWPawnData>())
		{
			if (const UOWInputConfig* InputConfig = PawnData->InputConfig)
			{
				int32 AddedMappingCount = 0;
				FModifyContextOptions Options;
				Options.bIgnoreAllPressedKeysUntilRelease = false;

				// HeroComponent가 가진 기본 Input Mapping Context를 EnhancedInputLocalPlayerSubsystem에 추가한다.
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
						++AddedMappingCount;
					}
					else
					{
						OW_WARN(LogOWGame, TEXT("InitializePlayerInput failed to load default InputMapping. Priority=%d"), Mapping.Priority);
					}
				}

				UOWInputComponent* OWInputComponent = Cast<UOWInputComponent>(InPlayerInputComponent);
				if (!OWInputComponent)
				{
					OW_WARN(LogOWGame, TEXT("InitializePlayerInput aborted: InputComponent is not UOWInputComponent. InputComponent=%s Class=%s"),
						*GetNameSafe(InPlayerInputComponent),
						*GetNameSafe(InPlayerInputComponent->GetClass()));
					return;
				}

				TArray<uint32> BindHandles;

				// AbilityInputActions는 GameplayTag를 ASC에 전달하여 GameplayAbility가 직접 입력으로 활성화되게 한다.
				OWInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);

				// NativeInputActions는 이동/시야/자세처럼 C++에서 직접 처리할 입력에 바인딩한다.
				OWInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::Get().GetInputTagMove(), ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
				OWInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::Get().GetInputTagLookMouse(), ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, false);
				OWInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::Get().GetInputTagLookStick(), ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, false);
				OWInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::Get().GetInputTagCrouch(), ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, false);

				OW_LOG(LogOWGame, Log, TEXT("InitializePlayerInput completed binding. PawnData=%s InputConfig=%s AddedMappings=%d AbilityBindHandles=%d NativeActions=%d"),
					*GetNameSafe(PawnData),
					*GetNameSafe(InputConfig),
					AddedMappingCount,
					BindHandles.Num(),
					InputConfig->NativeInputActions.Num());
			}
			else
			{
				OW_WARN(LogOWGame, TEXT("InitializePlayerInput skipped binding: PawnData InputConfig is null. PawnData=%s"), *GetNameSafe(PawnData));
			}
		}
		else
		{
			OW_WARN(LogOWGame, TEXT("InitializePlayerInput skipped binding: PawnExtension PawnData is null. Pawn=%s"), *GetNameSafe(Pawn));
		}
	}
	else
	{
		OW_WARN(LogOWGame, TEXT("InitializePlayerInput skipped binding: PawnExtensionComponent is missing. Pawn=%s"), *GetNameSafe(Pawn));
	}

	bReadyToBindInputs = true;
	OW_LOG(LogOWGame, Log, TEXT("InitializePlayerInput broadcasting BindInputsNow. Pawn=%s PlayerController=%s"),
		*GetNameSafe(Pawn),
		*GetNameSafe(PlayerController));

	// GameFeatureAction_AddInputContextMapping/AddInputBinding의 확장 콜백을 호출한다.
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PlayerController), NAME_BindInputsNow);
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UOWHeroComponent::AddAdditionalInputConfig(const UOWInputConfig* InInputConfig)
{
	if (!InInputConfig || AdditionalInputConfigBindHandles.Contains(InInputConfig))
	{
		return;
	}

	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	UOWInputComponent* OWInputComponent = Cast<UOWInputComponent>(Pawn->InputComponent);
	if (!ensureMsgf(OWInputComponent, TEXT("Unexpected InputComponent class. Additional ability inputs require UOWInputComponent.")))
	{
		return;
	}

	TArray<uint32> BindHandles;

	// GameFeature로 추가되는 InputConfig는 Ability 입력 의미만 추가한다. MappingContext 등록은 별도 액션이 담당한다.
	OWInputComponent->BindAbilityActions(InInputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
	AdditionalInputConfigBindHandles.Add(InInputConfig, MoveTemp(BindHandles));
}

void UOWHeroComponent::RemoveAdditionalInputConfig(const UOWInputConfig* InInputConfig)
{
	TArray<uint32> BindHandles;
	if (!InInputConfig || !AdditionalInputConfigBindHandles.RemoveAndCopyValue(InInputConfig, BindHandles))
	{
		return;
	}

	const APawn* Pawn = GetPawn<APawn>();
	UOWInputComponent* OWInputComponent = Pawn ? Cast<UOWInputComponent>(Pawn->InputComponent) : nullptr;
	if (!OWInputComponent)
	{
		return;
	}

	for (const uint32 BindHandle : BindHandles)
	{
		OWInputComponent->RemoveBindingByHandle(BindHandle);
	}
}

bool UOWHeroComponent::IsReadyToBindInputs() const
{
	return bReadyToBindInputs;
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
		// Left/Right 입력은 카메라 Yaw 기준 RightVector 방향 이동으로 변환한다.
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Forward/Backward 입력은 카메라 Yaw 기준 ForwardVector 방향 이동으로 변환한다.
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void UOWHeroComponent::Input_LookStick(const FInputActionValue& InInputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector2D Value = InInputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		// Stick 입력은 프레임 시간과 회전 속도를 곱해 일정한 회전률로 처리한다.
		Pawn->AddControllerYawInput(Value.X * LookYawRate * World->GetDeltaSeconds());
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y * LookPitchRate * World->GetDeltaSeconds());
	}
}

void UOWHeroComponent::Input_Crouch(const FInputActionValue& InInputActionValue)
{
	ACharacter* Character = GetPawn<ACharacter>();
	if (!Character)
	{
		return;
	}

	if (Character->IsCrouched())
	{
		Character->UnCrouch();
	}
	else if (Character->CanCrouch())
	{
		Character->Crouch();
	}
}

void UOWHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InInputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UOWAbilitySystemComponent* AbilitySystemComponent = PawnExtensionComponent->GetOWAbilitySystemComponent())
			{
				// ASC가 GameplayTag와 매칭되는 AbilitySpec을 찾아 Pressed/Held 큐에 넣는다.
				AbilitySystemComponent->AbilityInputTagPressed(InInputTag);
			}
		}
	}
}

void UOWHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InInputTag)
{
	if (const APawn* Pawn = GetPawn<APawn>())
	{
		if (const UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
		{
			if (UOWAbilitySystemComponent* AbilitySystemComponent = PawnExtensionComponent->GetOWAbilitySystemComponent())
			{
				// ASC가 GameplayTag와 매칭되는 AbilitySpec을 찾아 Released 큐에 넣고 Held에서 제거한다.
				AbilitySystemComponent->AbilityInputTagReleased(InInputTag);
			}
		}
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
		// X축은 Yaw 입력이다.
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		// Y축은 Pitch 입력이며, 마우스 입력은 프로젝트 기준에 맞게 반전한다.
		Pawn->AddControllerPitchInput(-Value.Y);
	}
}

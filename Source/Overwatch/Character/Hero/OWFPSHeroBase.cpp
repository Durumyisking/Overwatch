// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Hero/OWFPSHeroBase.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Character/Components/OWPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Core/Types/OWGameplayTags.h"
#include "Data/OWHeroData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Input/OWInputComponent.h"
#include "Input/OWInputConfig.h"
#include "InputActionValue.h"
#include "OWLog.h"
#include "Camera/OWCameraComponent.h"
#include "Player/OWPlayerState.h"

AOWFPSHeroBase::AOWFPSHeroBase()
{
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 96.0f);

	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	CameraComponent = CreateDefaultSubobject<UOWCameraComponent>(TEXT("First Person Camera"));
	CameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	CameraComponent->bUsePawnControlRotation = true;
	CameraComponent->bEnableFirstPersonFieldOfView = true;
	CameraComponent->bEnableFirstPersonScale = true;

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMeshComponent->SetupAttachment(FirstPersonMesh);
	WeaponMeshComponent->SetOnlyOwnerSee(true);
	WeaponMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AOWFPSHeroBase::PossessedBy(AController* InNewController)
{
	Super::PossessedBy(InNewController);

	AOWPlayerState* OWPlayerState = GetPlayerState<AOWPlayerState>();
	if (!OWPlayerState)
	{
		return;
	}

	if (UOWAbilitySystemComponent* AbilitySystemComponent = OWPlayerState->GetAbilitySystemComponent())
	{
		AbilitySystemComponent->InitAbilityActorInfo(OWPlayerState, this);
	}

	if (HasAuthority())
	{
		const FOWHeroData* HeroData = HeroDataRowHandle.GetRow<FOWHeroData>(TEXT("AOWFPSHeroBase::PossessedBy"));
		OWPlayerState->SetHeroData(HeroData);
	}
}

void AOWFPSHeroBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AOWPlayerState* OWPlayerState = GetPlayerState<AOWPlayerState>();
	if (!OWPlayerState)
	{
		return;
	}

	if (UOWAbilitySystemComponent* AbilitySystemComponent = OWPlayerState->GetAbilitySystemComponent())
	{
		AbilitySystemComponent->InitAbilityActorInfo(OWPlayerState, this);
	}
}

void AOWFPSHeroBase::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	UOWInputComponent* OWInputComponent = Cast<UOWInputComponent>(InInputComponent);
	if (!OWInputComponent)
	{
		UE_LOG(LogOWGame, Error, TEXT("'%s' Failed to find a UOWInputComponent."), *GetNameSafe(this));
		return;
	}

	if (!InputConfig)
	{
		OW_ACTOR_LOG(LogOWGame, Error, this, "InputConfig is not set.");
		return;
	}

	BindTaggedInputActions(OWInputComponent);

	if (PawnExtComponent)
	{
		PawnExtComponent->SetupPlayerInputComponent();
	}
}

void AOWFPSHeroBase::BindTaggedInputActions(UOWInputComponent* InInputComponent)
{
	check(InInputComponent);
	check(InputConfig);

	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagCrouch(), ETriggerEvent::Started, this, &ThisClass::DoCrouchStart);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagCrouch(), ETriggerEvent::Completed, this, &ThisClass::DoCrouchEnd);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagJump(), ETriggerEvent::Started, this, &ThisClass::DoJumpStart);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagJump(), ETriggerEvent::Completed, this, &ThisClass::DoJumpEnd);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagMove(), ETriggerEvent::Triggered, this, &ThisClass::MoveInput);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagLook(), ETriggerEvent::Triggered, this, &ThisClass::LookInput);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagLookMouse(), ETriggerEvent::Triggered, this, &ThisClass::LookInput, false);
}

void AOWFPSHeroBase::MoveInput(const FInputActionValue& InValue)
{
	const FVector2D MovementVector = InValue.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AOWFPSHeroBase::LookInput(const FInputActionValue& InValue)
{
	const FVector2D LookAxisVector = InValue.Get<FVector2D>();
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void AOWFPSHeroBase::DoAim(float InYaw, float InPitch)
{
	if (GetController())
	{
		AddControllerYawInput(InYaw);
		AddControllerPitchInput(InPitch);
	}
}

void AOWFPSHeroBase::DoMove(float InRight, float InForward)
{
	if (GetController())
	{
		AddMovementInput(GetActorRightVector(), InRight);
		AddMovementInput(GetActorForwardVector(), InForward);
	}
}

void AOWFPSHeroBase::DoJumpStart()
{
	Jump();
}

void AOWFPSHeroBase::DoJumpEnd()
{
	StopJumping();
}

void AOWFPSHeroBase::DoCrouchStart()
{
	Crouch();
}

void AOWFPSHeroBase::DoCrouchEnd()
{
	UnCrouch();
}

void AOWFPSHeroBase::DoShoot()
{
	OW_ACTOR_LOG(LogOWGame, Log, this, "Shoot input triggered.");
}

void AOWFPSHeroBase::DoReload()
{
	OW_ACTOR_LOG(LogOWGame, Log, this, "Reload input triggered.");
}

void AOWFPSHeroBase::DoSkill1()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Skill1 input triggered.");
}

void AOWFPSHeroBase::DoSkill2()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Skill2 input triggered.");
}

void AOWFPSHeroBase::DoSkill3()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Skill3 input triggered.");
}

void AOWFPSHeroBase::DoUltimate()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Ultimate input triggered.");
}

// Copyright Epic Games, Inc. All Rights Reserved.

#include "OWCharacterBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/Types/OWGameplayTags.h"
#include "Input/OWInputComponent.h"
#include "Input/OWInputConfig.h"
#include "OWLog.h"

AOWCharacterBase::AOWCharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// Create the weapon mesh that will be viewed only by this character's owner
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	WeaponMeshComponent->SetupAttachment(FirstPersonCameraComponent);
	WeaponMeshComponent->SetOnlyOwnerSee(true);
	WeaponMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void AOWCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UOWInputComponent* OWInputComponent = Cast<UOWInputComponent>(PlayerInputComponent))
	{
		if (InputConfig)
		{
			BindTaggedInputActions(OWInputComponent);
			return;
		}

		OW_ACTOR_LOG(LogOWGame, Warning, this, "InputConfig is not set. Falling back to legacy direct InputAction bindings.");
		BindLegacyInputActions(OWInputComponent);
		return;
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		OW_ACTOR_LOG(LogOWGame, Warning, this, "PlayerInputComponent is not UOWInputComponent. Falling back to legacy direct InputAction bindings.");
		BindLegacyInputActions(EnhancedInputComponent);
		return;
	}

	UE_LOG(LogOWGame, Error, TEXT("'%s' Failed to find an Enhanced Input Component."), *GetNameSafe(this));
}

void AOWCharacterBase::BindTaggedInputActions(UOWInputComponent* InInputComponent)
{
	check(InInputComponent);
	check(InputConfig);

	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagCrouch(), ETriggerEvent::Started, this, &AOWCharacterBase::DoCrouchStart);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagCrouch(), ETriggerEvent::Completed, this, &AOWCharacterBase::DoCrouchEnd);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagJump(), ETriggerEvent::Started, this, &AOWCharacterBase::DoJumpStart);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagJump(), ETriggerEvent::Completed, this, &AOWCharacterBase::DoJumpEnd);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagMove(), ETriggerEvent::Triggered, this, &AOWCharacterBase::MoveInput);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagLook(), ETriggerEvent::Triggered, this, &AOWCharacterBase::LookInput);
	InInputComponent->BindNativeAction(InputConfig, FOWGameplayTags::GetInputTagLookMouse(), ETriggerEvent::Triggered, this, &AOWCharacterBase::LookInput, false);
	InInputComponent->BindAbilityActions(InputConfig, ETriggerEvent::Started, this, &AOWCharacterBase::HandleAbilityInputPressed);
}

void AOWCharacterBase::BindLegacyInputActions(UEnhancedInputComponent* InInputComponent)
{
	check(InInputComponent);

	InInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AOWCharacterBase::DoCrouchStart);
	InInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AOWCharacterBase::DoCrouchEnd);
	InInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOWCharacterBase::DoJumpStart);
	InInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AOWCharacterBase::DoJumpEnd);
	InInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOWCharacterBase::MoveInput);
	InInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOWCharacterBase::LookInput);
	InInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AOWCharacterBase::LookInput);
	InInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AOWCharacterBase::DoShoot);
	InInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AOWCharacterBase::DoReload);
	InInputComponent->BindAction(Skill1Action, ETriggerEvent::Started, this, &AOWCharacterBase::DoSkill1);
	InInputComponent->BindAction(Skill2Action, ETriggerEvent::Started, this, &AOWCharacterBase::DoSkill2);
	InInputComponent->BindAction(Skill3Action, ETriggerEvent::Started, this, &AOWCharacterBase::DoSkill3);
	InInputComponent->BindAction(UltimateAction, ETriggerEvent::Started, this, &AOWCharacterBase::DoUltimate);
}

void AOWCharacterBase::HandleAbilityInputPressed(FGameplayTag InInputTag)
{
	if (InInputTag == FOWGameplayTags::GetInputTagShoot())
	{
		DoShoot();
		return;
	}

	if (InInputTag == FOWGameplayTags::GetInputTagReload())
	{
		DoReload();
		return;
	}

	if (InInputTag == FOWGameplayTags::GetInputTagAbilitySkill1())
	{
		DoSkill1();
		return;
	}

	if (InInputTag == FOWGameplayTags::GetInputTagAbilitySkill2())
	{
		DoSkill2();
		return;
	}

	if (InInputTag == FOWGameplayTags::GetInputTagAbilitySkill3())
	{
		DoSkill3();
		return;
	}

	if (InInputTag == FOWGameplayTags::GetInputTagAbilityUltimate())
	{
		DoUltimate();
		return;
	}

	OW_ACTOR_LOG(LogOWAbilitySystem, Warning, this, "Unhandled ability input tag: %s", *InInputTag.ToString());
}


void AOWCharacterBase::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);

}

void AOWCharacterBase::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);

}

void AOWCharacterBase::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AOWCharacterBase::DoMove(float Right, float Forward)
{
	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void AOWCharacterBase::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void AOWCharacterBase::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}

void AOWCharacterBase::DoCrouchStart()
{
	Crouch();
}

void AOWCharacterBase::DoCrouchEnd()
{
	UnCrouch();
}

void AOWCharacterBase::DoShoot()
{
	OW_ACTOR_LOG(LogOWGame, Log, this, "Shoot input triggered.");
}

void AOWCharacterBase::DoReload()
{
	OW_ACTOR_LOG(LogOWGame, Log, this, "Reload input triggered.");
}

void AOWCharacterBase::DoSkill1()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Skill1 input triggered.");
}

void AOWCharacterBase::DoSkill2()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Skill2 input triggered.");
}

void AOWCharacterBase::DoSkill3()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Skill3 input triggered.");
}

void AOWCharacterBase::DoUltimate()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Ultimate input triggered.");
}

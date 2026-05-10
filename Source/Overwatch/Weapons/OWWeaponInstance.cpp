#include "Weapons/OWWeaponInstance.h"

#include "Engine/World.h"
#include "GameFramework/InputDeviceProperties.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWWeaponInstance)

UOWWeaponInstance::UOWWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOWWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	UWorld* World = GetWorld();
	check(World);

	TimeLastEquipped = World->GetTimeSeconds();
	ApplyDeviceProperties();
}

void UOWWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();

	RemoveDeviceProperties();
}

void UOWWeaponInstance::UpdateFiringTime()
{
	UWorld* World = GetWorld();
	check(World);

	TimeLastFired = World->GetTimeSeconds();
}

float UOWWeaponInstance::GetTimeSinceLastInteractedWith() const
{
	UWorld* World = GetWorld();
	check(World);

	const double WorldTime = World->GetTimeSeconds();
	double Result = WorldTime - TimeLastEquipped;

	if (TimeLastFired > 0.0)
	{
		Result = FMath::Min(Result, WorldTime - TimeLastFired);
	}

	return Result;
}

TSubclassOf<UAnimInstance> UOWWeaponInstance::PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const
{
	const FOWAnimLayerSelectionSet& SetToQuery = bEquipped ? EquippedAnimSet : UnequippedAnimSet;
	return SetToQuery.SelectBestLayer(CosmeticTags);
}

FPlatformUserId UOWWeaponInstance::GetOwningUserId() const
{
	if (const APawn* Pawn = GetPawn())
	{
		return Pawn->GetPlatformUserId();
	}

	return PLATFORMUSERID_NONE;
}

void UOWWeaponInstance::ApplyDeviceProperties()
{
	const FPlatformUserId UserId = GetOwningUserId();
	if (!UserId.IsValid())
	{
		return;
	}

	UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get();
	if (!InputDeviceSubsystem)
	{
		return;
	}

	for (TObjectPtr<UInputDeviceProperty>& DeviceProperty : ApplicableDeviceProperties)
	{
		if (!DeviceProperty)
		{
			continue;
		}

		FActivateDevicePropertyParams Params;
		Params.UserId = UserId;
		Params.bLooping = true;

		DevicePropertyHandles.Emplace(InputDeviceSubsystem->ActivateDeviceProperty(DeviceProperty, Params));
	}
}

void UOWWeaponInstance::RemoveDeviceProperties()
{
	const FPlatformUserId UserId = GetOwningUserId();
	if (!UserId.IsValid() || DevicePropertyHandles.IsEmpty())
	{
		return;
	}

	if (UInputDeviceSubsystem* InputDeviceSubsystem = UInputDeviceSubsystem::Get())
	{
		InputDeviceSubsystem->RemoveDevicePropertyHandles(DevicePropertyHandles);
		DevicePropertyHandles.Empty();
	}
}

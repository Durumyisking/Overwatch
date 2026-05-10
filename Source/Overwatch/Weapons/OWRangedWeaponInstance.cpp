#include "Weapons/OWRangedWeaponInstance.h"

#include "Camera/OWCameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "NativeGameplayTags.h"
#include "Physics/OWPhysicalMaterialWithTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWRangedWeaponInstance)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_OW_Weapon_SteadyAimingCamera, "OW.Weapon.SteadyAimingCamera");

UOWRangedWeaponInstance::UOWRangedWeaponInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	HeatToHeatPerShotCurve.EditorCurveData.AddKey(0.0f, 1.0f);
	HeatToCoolDownPerSecondCurve.EditorCurveData.AddKey(0.0f, 2.0f);
}

void UOWRangedWeaponInstance::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

#if WITH_EDITOR
void UOWRangedWeaponInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UpdateDebugVisualization();
}

void UOWRangedWeaponInstance::UpdateDebugVisualization()
{
	ComputeHeatRange(Debug_MinHeat, Debug_MaxHeat);
	ComputeSpreadRange(Debug_MinSpreadAngle, Debug_MaxSpreadAngle);
	Debug_CurrentHeat = CurrentHeat;
	Debug_CurrentSpreadAngle = CurrentSpreadAngle;
	Debug_CurrentSpreadAngleMultiplier = CurrentSpreadAngleMultiplier;
}
#endif

void UOWRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	float MinHeatRange = 0.0f;
	float MaxHeatRange = 0.0f;
	ComputeHeatRange(MinHeatRange, MaxHeatRange);
	CurrentHeat = (MinHeatRange + MaxHeatRange) * 0.5f;

	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentSpreadAngleMultiplier = 1.0f;
	StandingStillMultiplier = 1.0f;
	JumpFallMultiplier = 1.0f;
	CrouchingMultiplier = 1.0f;
}

void UOWRangedWeaponInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void UOWRangedWeaponInstance::Tick(float InDeltaSeconds)
{
	if (!GetPawn())
	{
		return;
	}

	const bool bMinSpread = UpdateSpread(InDeltaSeconds);
	const bool bMinMultipliers = UpdateMultipliers(InDeltaSeconds);
	bHasFirstShotAccuracy = bAllowFirstShotAccuracy && bMinMultipliers && bMinSpread;

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

void UOWRangedWeaponInstance::AddSpread()
{
	const float HeatPerShot = HeatToHeatPerShotCurve.GetRichCurveConst()->Eval(CurrentHeat);
	CurrentHeat = ClampHeat(CurrentHeat + HeatPerShot);
	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);

	if (UWorld* World = GetWorld())
	{
		LastFireTime = World->GetTimeSeconds();
	}

#if WITH_EDITOR
	UpdateDebugVisualization();
#endif
}

float UOWRangedWeaponInstance::GetDistanceAttenuation(float InDistance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
	return Curve->HasAnyData() ? Curve->Eval(InDistance) : 1.0f;
}

float UOWRangedWeaponInstance::GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
	float CombinedMultiplier = 1.0f;
	if (const UOWPhysicalMaterialWithTags* PhysicalMaterialWithTags = Cast<const UOWPhysicalMaterialWithTags>(PhysicalMaterial))
	{
		for (const FGameplayTag MaterialTag : PhysicalMaterialWithTags->Tags)
		{
			if (const float* TagMultiplier = MaterialDamageMultiplier.Find(MaterialTag))
			{
				CombinedMultiplier *= *TagMultiplier;
			}
		}
	}

	return CombinedMultiplier;
}

void UOWRangedWeaponInstance::ComputeHeatRange(float& OutMinHeat, float& OutMaxHeat) const
{
	float MinHeatPerShot = 0.0f;
	float MaxHeatPerShot = 0.0f;
	HeatToHeatPerShotCurve.GetRichCurveConst()->GetTimeRange(MinHeatPerShot, MaxHeatPerShot);

	float MinCooldown = 0.0f;
	float MaxCooldown = 0.0f;
	HeatToCoolDownPerSecondCurve.GetRichCurveConst()->GetTimeRange(MinCooldown, MaxCooldown);

	float MinSpread = 0.0f;
	float MaxSpread = 0.0f;
	HeatToSpreadCurve.GetRichCurveConst()->GetTimeRange(MinSpread, MaxSpread);

	OutMinHeat = FMath::Min(FMath::Min(MinHeatPerShot, MinCooldown), MinSpread);
	OutMaxHeat = FMath::Max(FMath::Max(MaxHeatPerShot, MaxCooldown), MaxSpread);
}

void UOWRangedWeaponInstance::ComputeSpreadRange(float& OutMinSpread, float& OutMaxSpread) const
{
	HeatToSpreadCurve.GetRichCurveConst()->GetValueRange(OutMinSpread, OutMaxSpread);
}

float UOWRangedWeaponInstance::ClampHeat(float InNewHeat) const
{
	float MinHeat = 0.0f;
	float MaxHeat = 0.0f;
	ComputeHeatRange(MinHeat, MaxHeat);

	return FMath::Clamp(InNewHeat, MinHeat, MaxHeat);
}

bool UOWRangedWeaponInstance::UpdateSpread(float InDeltaSeconds)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const float TimeSinceFired = World->TimeSince(LastFireTime);
	if (TimeSinceFired > SpreadRecoveryCooldownDelay)
	{
		const float CooldownRate = HeatToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CurrentHeat);
		CurrentHeat = ClampHeat(CurrentHeat - (CooldownRate * InDeltaSeconds));
		CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	}

	float MinSpread = 0.0f;
	float MaxSpread = 0.0f;
	ComputeSpreadRange(MinSpread, MaxSpread);

	return FMath::IsNearlyEqual(CurrentSpreadAngle, MinSpread, KINDA_SMALL_NUMBER);
}

bool UOWRangedWeaponInstance::UpdateMultipliers(float InDeltaSeconds)
{
	const float MultiplierNearlyEqualThreshold = 0.05f;

	APawn* Pawn = GetPawn();
	if (!Pawn)
	{
		return false;
	}

	UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());

	const float PawnSpeed = Pawn->GetVelocity().Size();
	const float MovementTargetValue = FMath::GetMappedRangeValueClamped(
		FVector2D(StandingStillSpeedThreshold, StandingStillSpeedThreshold + StandingStillToMovingSpeedRange),
		FVector2D(SpreadAngleMultiplier_StandingStill, 1.0f),
		PawnSpeed);
	StandingStillMultiplier = FMath::FInterpTo(StandingStillMultiplier, MovementTargetValue, InDeltaSeconds, TransitionRate_StandingStill);
	const bool bStandingStillMultiplierAtMin = FMath::IsNearlyEqual(StandingStillMultiplier, SpreadAngleMultiplier_StandingStill, SpreadAngleMultiplier_StandingStill * 0.1f);

	const bool bIsCrouching = CharacterMovementComponent && CharacterMovementComponent->IsCrouching();
	const float CrouchingTargetValue = bIsCrouching ? SpreadAngleMultiplier_Crouching : 1.0f;
	CrouchingMultiplier = FMath::FInterpTo(CrouchingMultiplier, CrouchingTargetValue, InDeltaSeconds, TransitionRate_Crouching);
	const bool bCrouchingMultiplierAtTarget = FMath::IsNearlyEqual(CrouchingMultiplier, CrouchingTargetValue, MultiplierNearlyEqualThreshold);

	const bool bIsJumpingOrFalling = CharacterMovementComponent && CharacterMovementComponent->IsFalling();
	const float JumpFallTargetValue = bIsJumpingOrFalling ? SpreadAngleMultiplier_JumpingOrFalling : 1.0f;
	JumpFallMultiplier = FMath::FInterpTo(JumpFallMultiplier, JumpFallTargetValue, InDeltaSeconds, TransitionRate_JumpingOrFalling);
	const bool bJumpFallMultiplierIsOne = FMath::IsNearlyEqual(JumpFallMultiplier, 1.0f, MultiplierNearlyEqualThreshold);

	float AimingAlpha = 0.0f;
	if (const UOWCameraComponent* CameraComponent = UOWCameraComponent::FindCameraComponent(Pawn))
	{
		float TopCameraWeight = 0.0f;
		FGameplayTag TopCameraTag;
		CameraComponent->GetBlendInfo(TopCameraWeight, TopCameraTag);

		AimingAlpha = TopCameraTag == TAG_OW_Weapon_SteadyAimingCamera ? TopCameraWeight : 0.0f;
	}

	const float AimingMultiplier = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(1.0f, SpreadAngleMultiplier_Aiming), AimingAlpha);
	const bool bAimingMultiplierAtTarget = FMath::IsNearlyEqual(AimingMultiplier, SpreadAngleMultiplier_Aiming, KINDA_SMALL_NUMBER);

	CurrentSpreadAngleMultiplier = AimingMultiplier * StandingStillMultiplier * CrouchingMultiplier * JumpFallMultiplier;
	return bStandingStillMultiplierAtMin && bCrouchingMultiplierAtTarget && bJumpFallMultiplierIsOne && bAimingMultiplierAtTarget;
}

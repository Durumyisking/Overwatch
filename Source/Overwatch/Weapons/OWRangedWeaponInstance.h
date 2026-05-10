#pragma once

#include "AbilitySystem/OWAbilitySourceInterface.h"
#include "Curves/CurveFloat.h"
#include "Weapons/OWWeaponInstance.h"
#include "OWRangedWeaponInstance.generated.h"

class UPhysicalMaterial;

/** 탄 퍼짐, 거리 감쇠, 물리재질 보정을 소유하는 원거리 무기 인스턴스다. */
UCLASS()
class OVERWATCH_API UOWRangedWeaponInstance : public UOWWeaponInstance, public IOWAbilitySourceInterface
{
	GENERATED_BODY()

public:
	UOWRangedWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	void UpdateDebugVisualization();
#endif

	int32 GetBulletsPerCartridge() const { return BulletsPerCartridge; }
	float GetCalculatedSpreadAngle() const { return CurrentSpreadAngle; }
	float GetCalculatedSpreadAngleMultiplier() const { return bHasFirstShotAccuracy ? 0.0f : CurrentSpreadAngleMultiplier; }
	bool HasFirstShotAccuracy() const { return bHasFirstShotAccuracy; }
	float GetSpreadExponent() const { return SpreadExponent; }
	float GetMaxDamageRange() const { return MaxDamageRange; }
	float GetBulletTraceSweepRadius() const { return BulletTraceSweepRadius; }

	void Tick(float InDeltaSeconds);
	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	/** 한 번 발사한 뒤 무기 heat를 올려 다음 탄 퍼짐에 반영한다. */
	void AddSpread();

	virtual float GetDistanceAttenuation(float InDistance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const override;

protected:
#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, Category = "Spread|Fire Params")
	float Debug_MinHeat = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spread|Fire Params")
	float Debug_MaxHeat = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spread|Fire Params", meta = (ForceUnits = deg))
	float Debug_MinSpreadAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spread|Fire Params", meta = (ForceUnits = deg))
	float Debug_MaxSpreadAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spread Debugging")
	float Debug_CurrentHeat = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spread Debugging", meta = (ForceUnits = deg))
	float Debug_CurrentSpreadAngle = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Spread Debugging", meta = (ForceUnits = x))
	float Debug_CurrentSpreadAngleMultiplier = 1.0f;
#endif

	/** 탄이 중심선 주변에 얼마나 몰릴지 결정한다. 1이면 균등 분포다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.1), Category = "Spread|Fire Params")
	float SpreadExponent = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToSpreadCurve;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToHeatPerShotCurve;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	FRuntimeFloatCurve HeatToCoolDownPerSecondCurve;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params", meta = (ForceUnits = s))
	float SpreadRecoveryCooldownDelay = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
	bool bAllowFirstShotAccuracy = false;

	UPROPERTY(EditAnywhere, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_Aiming = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_StandingStill = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_StandingStill = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = "cm/s"))
	float StandingStillSpeedThreshold = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = "cm/s"))
	float StandingStillToMovingSpeedRange = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_Crouching = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_Crouching = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
	float SpreadAngleMultiplier_JumpingOrFalling = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
	float TransitionRate_JumpingOrFalling = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config")
	int32 BulletsPerCartridge = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
	float MaxDamageRange = 25000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
	float BulletTraceSweepRadius = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
	FRuntimeFloatCurve DistanceDamageFalloff;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
	TMap<FGameplayTag, float> MaterialDamageMultiplier;

private:
	void ComputeSpreadRange(float& OutMinSpread, float& OutMaxSpread) const;
	void ComputeHeatRange(float& OutMinHeat, float& OutMaxHeat) const;
	float ClampHeat(float InNewHeat) const;
	bool UpdateSpread(float InDeltaSeconds);
	bool UpdateMultipliers(float InDeltaSeconds);

	double LastFireTime = 0.0;
	float CurrentHeat = 0.0f;
	float CurrentSpreadAngle = 0.0f;
	bool bHasFirstShotAccuracy = false;
	float CurrentSpreadAngleMultiplier = 1.0f;
	float StandingStillMultiplier = 1.0f;
	float JumpFallMultiplier = 1.0f;
	float CrouchingMultiplier = 1.0f;
};

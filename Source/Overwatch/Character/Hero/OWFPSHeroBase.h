// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/Base/OWCharacterBase.h"
#include "Engine/DataTable.h"
#include "OWFPSHeroBase.generated.h"

class UOWCameraComponent;
class UCameraComponent;
class UOWInputComponent;
class UOWInputConfig;
class USkeletalMeshComponent;
struct FInputActionValue;

UCLASS(Abstract)
class OVERWATCH_API AOWFPSHeroBase : public AOWCharacterBase
{
	GENERATED_BODY()

public:
	AOWFPSHeroBase();

protected:
	virtual void PossessedBy(AController* InNewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

	void MoveInput(const FInputActionValue& InValue);
	void LookInput(const FInputActionValue& InValue);
	void BindTaggedInputActions(UOWInputComponent* InInputComponent);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAim(float InYaw, float InPitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float InRight, float InForward);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoCrouchStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoCrouchEnd();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoShoot();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoReload();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSkill1();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSkill2();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoSkill3();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoUltimate();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (RowType = "/Script/Overwatch.OWHeroData", AllowPrivateAccess = "true"))
	FDataTableRowHandle HeroDataRowHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOWInputConfig> InputConfig;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOWCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

public:
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	USkeletalMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; }
};

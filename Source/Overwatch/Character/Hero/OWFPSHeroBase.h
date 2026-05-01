// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/Base/OWCharacterBase.h"
#include "OWFPSHeroBase.generated.h"

class UOWCameraComponent;
class USkeletalMeshComponent;

UCLASS(Abstract)
class OVERWATCH_API AOWFPSHeroBase : public AOWCharacterBase
{
	GENERATED_BODY()

public:
	AOWFPSHeroBase();

protected:
	virtual void PossessedBy(AController* InNewController) override;
	virtual void OnRep_PlayerState() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;

public:
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	USkeletalMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; }
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "OWCharacterBase.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UOWInputComponent;
class UOWInputConfig;
struct FInputActionValue;

/**
 *  A basic first person character
 */
UCLASS(abstract)
class AOWCharacterBase : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Weapon mesh shown for the owning player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* WeaponMeshComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data", meta = (RowType="/Script/Overwatch.OWHeroData", AllowPrivateAccess = "true"))
	FDataTableRowHandle HeroDataRowHandle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOWInputConfig> InputConfig;

	// Legacy fallback until character assets are migrated to UOWInputConfig.
	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* CrouchAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	class UInputAction* MouseLookAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* ShootAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* ReloadAction;

	/** Skill1 Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* Skill1Action;

	/** Skill2 Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* Skill2Action;

	/** Skill3 Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* Skill3Action;

	/** Ultimate Input Action */
	UPROPERTY(EditAnywhere, Category ="Input|Legacy")
	UInputAction* UltimateAction;
	
public:
	AOWCharacterBase();

protected:

	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

	/** Handles crouch start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoCrouchStart();

	/** Handles crouch end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoCrouchEnd();

	/** Handles shoot inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoShoot();

	/** Handles reload inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoReload();

	/** Handles skill1 inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSkill1();

	/** Handles skill2 inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSkill2();

	/** Handles skill3 inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoSkill3();

	/** Handles ultimate inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoUltimate();

protected:
	void BindTaggedInputActions(UOWInputComponent* InInputComponent);
	void BindLegacyInputActions(class UEnhancedInputComponent* InInputComponent);
	void HandleAbilityInputPressed(FGameplayTag InInputTag);

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	

public:

	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	/** Returns weapon mesh component **/
	USkeletalMeshComponent* GetWeaponMeshComponent() const { return WeaponMeshComponent; }

};


// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OWCharacterBase.generated.h"

class UOWPawnExtensionComponent;

UCLASS(abstract)
class OVERWATCH_API AOWCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOWCharacterBase();

protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OW|Character")
	TObjectPtr<UOWPawnExtensionComponent> PawnExtComponent;
};


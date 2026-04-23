// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "OWCharacterBase.generated.h"

class UOWPawnExtensionComponent;

UCLASS(abstract)
class AOWCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AOWCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hak|Character")
	TObjectPtr<UOWPawnExtensionComponent> PawnExtComponent;
};


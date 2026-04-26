// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Base/OWCharacterBase.h"

#include "Character/Components/OWPawnExtensionComponent.h"

AOWCharacterBase::AOWCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UOWPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}

void AOWCharacterBase::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	// PawnExtension drives the Lyra-style init-state chain once the input component exists.
	if (PawnExtComponent)
	{
		PawnExtComponent->SetupPlayerInputComponent();
	}
}

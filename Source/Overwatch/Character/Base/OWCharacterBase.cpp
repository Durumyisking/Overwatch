// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Base/OWCharacterBase.h"

#include "Character/Components/OWPawnExtensionComponent.h"

AOWCharacterBase::AOWCharacterBase()
{
	PawnExtComponent = CreateDefaultSubobject<UOWPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
}

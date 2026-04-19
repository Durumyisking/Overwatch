// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Hero/Soldier76/OWHeroSoldier76.h"

#include "OWLog.h"

AOWHeroSoldier76::AOWHeroSoldier76()
{
}

void AOWHeroSoldier76::DoShoot()
{
	OW_ACTOR_LOG(LogOWGame, Log, this, "Soldier76 primary fire input triggered.");
}

void AOWHeroSoldier76::DoReload()
{
	OW_ACTOR_LOG(LogOWGame, Log, this, "Soldier76 reload input triggered.");
}

void AOWHeroSoldier76::DoSkill1()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Soldier76 sprint input triggered.");
}

void AOWHeroSoldier76::DoSkill2()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Soldier76 helix rockets input triggered.");
}

void AOWHeroSoldier76::DoSkill3()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Soldier76 biotic field input triggered.");
}

void AOWHeroSoldier76::DoUltimate()
{
	OW_ACTOR_LOG(LogOWAbilitySystem, Log, this, "Soldier76 tactical visor input triggered.");
}

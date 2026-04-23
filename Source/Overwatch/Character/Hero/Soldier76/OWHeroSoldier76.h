// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/Hero/OWFPSHeroBase.h"
#include "OWHeroSoldier76.generated.h"

UCLASS()
class OVERWATCH_API AOWHeroSoldier76 : public AOWFPSHeroBase
{
	GENERATED_BODY()

public:
	AOWHeroSoldier76();

protected:
	virtual void DoShoot() override;
	virtual void DoReload() override;
	virtual void DoSkill1() override;
	virtual void DoSkill2() override;
	virtual void DoSkill3() override;
	virtual void DoUltimate() override;
};

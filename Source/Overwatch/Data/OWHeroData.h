#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OWHeroData.generated.h"

class UOWAbilitySet;
class USkeletalMesh;

USTRUCT(BlueprintType)
struct OVERWATCH_API FOWHeroData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	int32 WeaponDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UOWAbilitySet> AbilitySet;
};

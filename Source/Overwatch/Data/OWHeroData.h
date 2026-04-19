#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "OWHeroData.generated.h"

class UOWAbilitySet;

USTRUCT(BlueprintType)
struct OVERWATCH_API FOWHeroData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UOWAbilitySet> AbilitySet;
};

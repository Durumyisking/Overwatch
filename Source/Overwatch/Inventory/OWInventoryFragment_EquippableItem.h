#pragma once

#include "Inventory/OWInventoryItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "OWInventoryFragment_EquippableItem.generated.h"

class UOWEquipmentDefinition;

/** 이 아이템이 QuickBar에서 장착될 때 사용할 EquipmentDefinition을 지정한다. */
UCLASS()
class OVERWATCH_API UOWInventoryFragment_EquippableItem : public UOWInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<UOWEquipmentDefinition> EquipmentDefinition;
};

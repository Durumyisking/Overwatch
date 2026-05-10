#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/OWInventoryItemDefinition.h"
#include "OWInventoryFragment_SetStats.generated.h"

class UOWInventoryItemInstance;

/** 아이템 인스턴스가 생성될 때 초기 StatTag 스택을 부여하는 Fragment다. */
UCLASS()
class OVERWATCH_API UOWInventoryFragment_SetStats : public UOWInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UOWInventoryItemInstance* Instance) const override;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetItemStatByTag(FGameplayTag InTag) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TMap<FGameplayTag, int32> InitialItemStats;
};

#include "Inventory/OWInventoryFragment_SetStats.h"

#include "Inventory/OWInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWInventoryFragment_SetStats)

void UOWInventoryFragment_SetStats::OnInstanceCreated(UOWInventoryItemInstance* Instance) const
{
	if (!Instance)
	{
		return;
	}

	for (const TPair<FGameplayTag, int32>& ItemStat : InitialItemStats)
	{
		Instance->AddStatTagStack(ItemStat.Key, ItemStat.Value);
	}
}

int32 UOWInventoryFragment_SetStats::GetItemStatByTag(FGameplayTag InTag) const
{
	if (const int32* StatValue = InitialItemStats.Find(InTag))
	{
		return *StatValue;
	}

	return 0;
}

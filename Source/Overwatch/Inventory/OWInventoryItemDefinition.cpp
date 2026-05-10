#include "Inventory/OWInventoryItemDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWInventoryItemDefinition)

UOWInventoryItemDefinition::UOWInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UOWInventoryItemFragment* UOWInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UOWInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass)
	{
		for (UOWInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

const UOWInventoryItemFragment* UOWInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, TSubclassOf<UOWInventoryItemFragment> FragmentClass)
{
	if (ItemDefinition && FragmentClass)
	{
		return GetDefault<UOWInventoryItemDefinition>(ItemDefinition)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

#include "Inventory/OWInventoryItemInstance.h"

#include "Inventory/OWInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWInventoryItemInstance)

class FLifetimeProperty;

UOWInventoryItemInstance::UOWInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOWInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void UOWInventoryItemInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

void UOWInventoryItemInstance::AddStatTagStack(FGameplayTag InTag, int32 InStackCount)
{
	StatTags.AddStack(InTag, InStackCount);
}

void UOWInventoryItemInstance::RemoveStatTagStack(FGameplayTag InTag, int32 InStackCount)
{
	StatTags.RemoveStack(InTag, InStackCount);
}

int32 UOWInventoryItemInstance::GetStatTagStackCount(FGameplayTag InTag) const
{
	return StatTags.GetStackCount(InTag);
}

bool UOWInventoryItemInstance::HasStatTag(FGameplayTag InTag) const
{
	return StatTags.ContainsTag(InTag);
}

void UOWInventoryItemInstance::SetItemDef(TSubclassOf<UOWInventoryItemDefinition> InItemDef)
{
	ItemDef = InItemDef;
}

const UOWInventoryItemFragment* UOWInventoryItemInstance::FindFragmentByClass(TSubclassOf<UOWInventoryItemFragment> FragmentClass) const
{
	if (ItemDef && FragmentClass)
	{
		return GetDefault<UOWInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

#include "Inventory/OWInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Inventory/OWInventoryItemDefinition.h"
#include "Inventory/OWInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWInventoryManagerComponent)

class FLifetimeProperty;

FString FOWInventoryEntry::GetDebugString() const
{
	TSubclassOf<UOWInventoryItemDefinition> ItemDefinition;
	if (Instance)
	{
		ItemDefinition = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDefinition.Get()));
}

void FOWInventoryList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		FOWInventoryEntry& Entry = Entries[Index];
		BroadcastChangeMessage(Entry, Entry.StackCount, 0);
		Entry.LastObservedCount = 0;
	}
}

void FOWInventoryList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		FOWInventoryEntry& Entry = Entries[Index];
		BroadcastChangeMessage(Entry, 0, Entry.StackCount);
		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FOWInventoryList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		FOWInventoryEntry& Entry = Entries[Index];
		check(Entry.LastObservedCount != INDEX_NONE);
		BroadcastChangeMessage(Entry, Entry.LastObservedCount, Entry.StackCount);
		Entry.LastObservedCount = Entry.StackCount;
	}
}

void FOWInventoryList::BroadcastChangeMessage(FOWInventoryEntry& Entry, int32 OldCount, int32 NewCount) const
{
	if (!OwnerComponent)
	{
		return;
	}

	FOWInventoryChangeMessage Message;
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Entry.Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;
	OwnerComponent->BroadcastChangeMessage(Message);
}

UOWInventoryItemInstance* FOWInventoryList::AddEntry(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	check(ItemDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	FOWInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = NewObject<UOWInventoryItemInstance>(OwnerComponent->GetOwner());
	NewEntry.Instance->SetItemDef(ItemDefinition);
	NewEntry.StackCount = StackCount;
	NewEntry.LastObservedCount = StackCount;

	const UOWInventoryItemDefinition* ItemCDO = GetDefault<UOWInventoryItemDefinition>(ItemDefinition);
	for (UOWInventoryItemFragment* Fragment : ItemCDO->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(NewEntry.Instance);
		}
	}

	MarkItemDirty(NewEntry);
	BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
	return NewEntry.Instance;
}

void FOWInventoryList::AddEntry(UOWInventoryItemInstance* Instance, int32 StackCount)
{
	check(Instance);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	FOWInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = Instance;
	NewEntry.StackCount = StackCount;
	NewEntry.LastObservedCount = StackCount;

	MarkItemDirty(NewEntry);
	BroadcastChangeMessage(NewEntry, 0, NewEntry.StackCount);
}

void FOWInventoryList::RemoveEntry(UOWInventoryItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FOWInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance != Instance)
		{
			continue;
		}

		BroadcastChangeMessage(Entry, Entry.StackCount, 0);
		EntryIt.RemoveCurrent();
		MarkArrayDirty();
		return;
	}
}

TArray<UOWInventoryItemInstance*> FOWInventoryList::GetAllItems() const
{
	TArray<UOWInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());

	for (const FOWInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance)
		{
			Results.Add(Entry.Instance);
		}
	}

	return Results;
}

UOWInventoryManagerComponent::UOWInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
}

void UOWInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

bool UOWInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 StackCount) const
{
	return ItemDefinition && StackCount > 0;
}

UOWInventoryItemInstance* UOWInventoryManagerComponent::AddItemDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	UOWInventoryItemInstance* Result = nullptr;
	if (CanAddItemDefinition(ItemDefinition, StackCount))
	{
		Result = InventoryList.AddEntry(ItemDefinition, StackCount);
		if (Result && IsUsingRegisteredSubObjectList() && IsReadyForReplication())
		{
			AddReplicatedSubObject(Result);
		}
	}

	return Result;
}

void UOWInventoryManagerComponent::AddItemInstance(UOWInventoryItemInstance* ItemInstance)
{
	if (!ItemInstance)
	{
		return;
	}

	InventoryList.AddEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
	{
		AddReplicatedSubObject(ItemInstance);
	}
}

void UOWInventoryManagerComponent::RemoveItemInstance(UOWInventoryItemInstance* ItemInstance)
{
	if (!ItemInstance)
	{
		return;
	}

	InventoryList.RemoveEntry(ItemInstance);
	if (IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}
}

TArray<UOWInventoryItemInstance*> UOWInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UOWInventoryItemInstance* UOWInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition) const
{
	for (const FOWInventoryEntry& Entry : InventoryList.Entries)
	{
		UOWInventoryItemInstance* Instance = Entry.Instance;
		if (IsValid(Instance) && Instance->GetItemDef() == ItemDefinition)
		{
			return Instance;
		}
	}

	return nullptr;
}

int32 UOWInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition) const
{
	int32 TotalCount = 0;
	for (const FOWInventoryEntry& Entry : InventoryList.Entries)
	{
		UOWInventoryItemInstance* Instance = Entry.Instance;
		if (IsValid(Instance) && Instance->GetItemDef() == ItemDefinition)
		{
			TotalCount += Entry.StackCount;
		}
	}

	return TotalCount;
}

bool UOWInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority() || !ItemDefinition || NumToConsume <= 0)
	{
		return false;
	}

	if (GetTotalItemCountByDefinition(ItemDefinition) < NumToConsume)
	{
		return false;
	}

	int32 RemainingCount = NumToConsume;
	for (auto EntryIt = InventoryList.Entries.CreateIterator(); EntryIt && RemainingCount > 0; ++EntryIt)
	{
		FOWInventoryEntry& Entry = *EntryIt;
		UOWInventoryItemInstance* Instance = Entry.Instance;
		if (!IsValid(Instance) || Instance->GetItemDef() != ItemDefinition)
		{
			continue;
		}

		const int32 OldCount = Entry.StackCount;
		const int32 CountToConsume = FMath::Min(RemainingCount, Entry.StackCount);
		Entry.StackCount -= CountToConsume;
		RemainingCount -= CountToConsume;

		if (Entry.StackCount <= 0)
		{
			InventoryList.BroadcastChangeMessage(Entry, OldCount, 0);
			if (IsUsingRegisteredSubObjectList())
			{
				RemoveReplicatedSubObject(Instance);
			}
			EntryIt.RemoveCurrent();
			InventoryList.MarkArrayDirty();
		}
		else
		{
			InventoryList.BroadcastChangeMessage(Entry, OldCount, Entry.StackCount);
			InventoryList.MarkItemDirty(Entry);
		}
	}

	return RemainingCount == 0;
}

void UOWInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FOWInventoryEntry& Entry : InventoryList.Entries)
		{
			if (IsValid(Entry.Instance))
			{
				AddReplicatedSubObject(Entry.Instance);
			}
		}
	}
}

bool UOWInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FOWInventoryEntry& Entry : InventoryList.Entries)
	{
		if (IsValid(Entry.Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Entry.Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UOWInventoryManagerComponent::BroadcastChangeMessage(const FOWInventoryChangeMessage& Message) const
{
	OnInventoryChanged.Broadcast(Message);
}

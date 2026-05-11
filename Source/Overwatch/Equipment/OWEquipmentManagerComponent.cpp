#include "Equipment/OWEquipmentManagerComponent.h"

#include "AbilitySystem/OWAbilitySystemComponent.h"
#include "Character/Components/OWPawnExtensionComponent.h"
#include "Engine/ActorChannel.h"
#include "Equipment/OWEquipmentDefinition.h"
#include "Equipment/OWEquipmentInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWEquipmentManagerComponent)

class FLifetimeProperty;

FString FOWAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

void FOWEquipmentList::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		const FOWAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance)
		{
			Entry.Instance->OnUnequipped();
		}
	}
}

void FOWEquipmentList::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FOWAppliedEquipmentEntry& Entry = Entries[Index];
		if (Entry.Instance)
		{
			Entry.Instance->OnEquipped();
		}
	}
}

void FOWEquipmentList::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
}

UOWAbilitySystemComponent* FOWEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);

	const AActor* OwningActor = OwnerComponent->GetOwner();
	const UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(OwningActor);
	return PawnExtensionComponent ? PawnExtensionComponent->GetOWAbilitySystemComponent() : nullptr;
}

UOWEquipmentInstance* FOWEquipmentList::AddEntry(TSubclassOf<UOWEquipmentDefinition> EquipmentDefinition)
{
	check(EquipmentDefinition);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	const UOWEquipmentDefinition* EquipmentCDO = GetDefault<UOWEquipmentDefinition>(EquipmentDefinition);
	TSubclassOf<UOWEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (!InstanceType)
	{
		InstanceType = UOWEquipmentInstance::StaticClass();
	}

	FOWAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<UOWEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);

	if (UOWAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
	{
		for (const TObjectPtr<const UOWAbilitySet>& AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			if (AbilitySet)
			{
				AbilitySet->GiveAbilitySystem(AbilitySystemComponent, &NewEntry.GrantedHandles, NewEntry.Instance);
			}
		}
	}

	NewEntry.Instance->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);
	MarkItemDirty(NewEntry);
	return NewEntry.Instance;
}

void FOWEquipmentList::RemoveEntry(UOWEquipmentInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FOWAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance != Instance)
		{
			continue;
		}

		if (UOWAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponent())
		{
			Entry.GrantedHandles.TakeFromAbilitySystem(AbilitySystemComponent);
		}

		Instance->DestroyEquipmentActors();
		EntryIt.RemoveCurrent();
		MarkArrayDirty();
		return;
	}
}

UOWEquipmentManagerComponent::UOWEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this) // Owner설정을 까먹지 말쟈
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UOWEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UOWEquipmentInstance* UOWEquipmentManagerComponent::EquipItem(TSubclassOf<UOWEquipmentDefinition> EquipmentDefinition)
{
	UOWEquipmentInstance* Result = nullptr;
	if (EquipmentDefinition)
	{
		Result = EquipmentList.AddEntry(EquipmentDefinition);
		if (Result)
		{
			Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}

	return Result;
}

void UOWEquipmentManagerComponent::UnequipItem(UOWEquipmentInstance* ItemInstance)
{
	if (!ItemInstance)
	{
		return;
	}

	if (IsUsingRegisteredSubObjectList())
	{
		RemoveReplicatedSubObject(ItemInstance);
	}

	ItemInstance->OnUnequipped();
	EquipmentList.RemoveEntry(ItemInstance);
}

bool UOWEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool bWroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FOWAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UOWEquipmentInstance* Instance = Entry.Instance;
		if (IsValid(Instance))
		{
			bWroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return bWroteSomething;
}

void UOWEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UOWEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UOWEquipmentInstance*> AllEquipmentInstances;
	for (const FOWAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	// Unequip 과정에서 목록이 바뀌므로 먼저 복사본을 만든 뒤 제거한다.
	for (UOWEquipmentInstance* EquipmentInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipmentInstance);
	}

	Super::UninitializeComponent();
}

void UOWEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	if (IsUsingRegisteredSubObjectList())
	{
		for (const FOWAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			if (IsValid(Entry.Instance))
			{
				AddReplicatedSubObject(Entry.Instance);
			}
		}
	}
}

UOWEquipmentInstance* UOWEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UOWEquipmentInstance> InstanceType)
{
	for (FOWAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UOWEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UOWEquipmentInstance*> UOWEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UOWEquipmentInstance> InstanceType) const
{
	TArray<UOWEquipmentInstance*> Results;
	for (const FOWAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UOWEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}

	return Results;
}

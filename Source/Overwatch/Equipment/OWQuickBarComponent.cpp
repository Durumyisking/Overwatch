#include "Equipment/OWQuickBarComponent.h"

#include "Equipment/OWEquipmentDefinition.h"
#include "Equipment/OWEquipmentInstance.h"
#include "Equipment/OWEquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "Inventory/OWInventoryFragment_EquippableItem.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWQuickBarComponent)

class FLifetimeProperty;

UOWQuickBarComponent::UOWQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UOWQuickBarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void UOWQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UOWQuickBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex;
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex])
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	}
	while (NewIndex != OldIndex);
}

void UOWQuickBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex;
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex])
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	}
	while (NewIndex != OldIndex);
}

void UOWQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (!Slots.IsValidIndex(NewIndex) || ActiveSlotIndex == NewIndex)
	{
		return;
	}

	UnequipItemInSlot();
	ActiveSlotIndex = NewIndex;
	EquipItemInSlot();
	OnRep_ActiveSlotIndex();
}

UOWInventoryItemInstance* UOWQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UOWQuickBarComponent::GetNextFreeItemSlot() const
{
	for (int32 SlotIndex = 0; SlotIndex < Slots.Num(); ++SlotIndex)
	{
		if (!Slots[SlotIndex])
		{
			return SlotIndex;
		}
	}

	return INDEX_NONE;
}

void UOWQuickBarComponent::AddItemToSlot(int32 SlotIndex, UOWInventoryItemInstance* Item)
{
	if (!Slots.IsValidIndex(SlotIndex) || !Item || Slots[SlotIndex])
	{
		return;
	}

	Slots[SlotIndex] = Item;
	OnRep_Slots();
}

UOWInventoryItemInstance* UOWQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UOWInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		ActiveSlotIndex = INDEX_NONE;
		OnRep_ActiveSlotIndex();
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];
		if (Result)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}

void UOWQuickBarComponent::UnequipItemInSlot()
{
	if (!EquippedItem)
	{
		return;
	}

	if (UOWEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		EquipmentManager->UnequipItem(EquippedItem);
	}

	EquippedItem = nullptr;
}

void UOWQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	UOWInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex];
	if (!SlotItem)
	{
		return;
	}

	const UOWInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UOWInventoryFragment_EquippableItem>();
	if (!EquipInfo || !EquipInfo->EquipmentDefinition)
	{
		return;
	}

	if (UOWEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		EquippedItem = EquipmentManager->EquipItem(EquipInfo->EquipmentDefinition);
		if (EquippedItem)
		{
			// EquipmentInstance는 자신을 만든 InventoryItemInstance를 알되, 슬롯 소유권은 QuickBar에 남긴다.
			EquippedItem->SetInstigator(SlotItem);
		}
	}
}

UOWEquipmentManagerComponent* UOWQuickBarComponent::FindEquipmentManager() const
{
	const AController* OwnerController = Cast<AController>(GetOwner());
	if (!OwnerController)
	{
		return nullptr;
	}

	APawn* Pawn = OwnerController->GetPawn();
	return Pawn ? Pawn->FindComponentByClass<UOWEquipmentManagerComponent>() : nullptr;
}

void UOWQuickBarComponent::OnRep_Slots()
{
	FOWQuickBarSlotsChangedMessage Message;
	Message.Owner = GetOwner();
	Message.Slots = Slots;
	OnSlotsChanged.Broadcast(Message);
}

void UOWQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FOWQuickBarActiveIndexChangedMessage Message;
	Message.Owner = GetOwner();
	Message.ActiveIndex = ActiveSlotIndex;
	OnActiveIndexChanged.Broadcast(Message);
}

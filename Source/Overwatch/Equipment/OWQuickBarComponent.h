#pragma once

#include "Components/ControllerComponent.h"
#include "Inventory/OWInventoryItemInstance.h"
#include "OWQuickBarComponent.generated.h"

class AActor;
class UOWEquipmentInstance;
class UOWEquipmentManagerComponent;

/** QuickBar 슬롯 변경 알림이다. */
USTRUCT(BlueprintType)
struct FOWQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TArray<TObjectPtr<UOWInventoryItemInstance>> Slots;
};

/** QuickBar 활성 슬롯 변경 알림이다. */
USTRUCT(BlueprintType)
struct FOWQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 ActiveIndex = INDEX_NONE;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOWQuickBarSlotsChanged, const FOWQuickBarSlotsChangedMessage&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOWQuickBarActiveIndexChanged, const FOWQuickBarActiveIndexChangedMessage&, Message);

/**
 * PlayerController가 소유하는 QuickBar 상태다.
 * Controller는 어떤 슬롯을 선택했는지만 소유하고, 실제 장착 상태는 Pawn의 EquipmentManager가 소유한다.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UOWQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "OW|QuickBar")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category = "OW|QuickBar")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "OW|QuickBar")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "OW|QuickBar")
	TArray<UOWInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "OW|QuickBar")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "OW|QuickBar")
	UOWInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "OW|QuickBar")
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "OW|QuickBar")
	void AddItemToSlot(int32 SlotIndex, UOWInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "OW|QuickBar")
	UOWInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "OW|QuickBar")
	FOWQuickBarSlotsChanged OnSlotsChanged;

	UPROPERTY(BlueprintAssignable, Category = "OW|QuickBar")
	FOWQuickBarActiveIndexChanged OnActiveIndexChanged;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();
	UOWEquipmentManagerComponent* FindEquipmentManager() const;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OW|QuickBar")
	int32 NumSlots = 3;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Slots)
	TArray<TObjectPtr<UOWInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = INDEX_NONE;

	UPROPERTY()
	TObjectPtr<UOWEquipmentInstance> EquippedItem;
};

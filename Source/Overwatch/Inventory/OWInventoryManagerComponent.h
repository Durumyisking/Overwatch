#pragma once

#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "OWInventoryManagerComponent.generated.h"

class UOWInventoryItemDefinition;
class UOWInventoryItemInstance;
class UOWInventoryManagerComponent;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** Inventory 항목 변화 알림용 메시지다. UI는 이 메시지를 관찰하고 상태 소유자가 되지 않는다. */
USTRUCT(BlueprintType)
struct FOWInventoryChangeMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UOWInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOWInventoryChanged, const FOWInventoryChangeMessage&, Message);

/** Inventory의 단일 아이템 스택 항목이다. */
USTRUCT(BlueprintType)
struct FOWInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FString GetDebugString() const;

private:
	friend struct FOWInventoryList;
	friend class UOWInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UOWInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};

/** Inventory 항목 목록을 FastArray로 복제한다. */
USTRUCT(BlueprintType)
struct FOWInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FOWInventoryList()
		: OwnerComponent(nullptr)
	{}

	FOWInventoryList(UOWInventoryManagerComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	TArray<UOWInventoryItemInstance*> GetAllItems() const;

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOWInventoryEntry, FOWInventoryList>(Entries, DeltaParms, *this);
	}

	UOWInventoryItemInstance* AddEntry(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 StackCount);
	void AddEntry(UOWInventoryItemInstance* Instance, int32 StackCount = 1);
	void RemoveEntry(UOWInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FOWInventoryEntry& Entry, int32 OldCount, int32 NewCount) const;

private:
	friend class UOWInventoryManagerComponent;

	UPROPERTY()
	TArray<FOWInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UOWInventoryManagerComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FOWInventoryList> : public TStructOpsTypeTraitsBase2<FOWInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};

/** Actor가 소유하는 Inventory 런타임 상태다. PlayerController 등에 붙여 플레이어 소유 아이템을 복제한다. */
UCLASS(BlueprintType, meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UOWInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool CanAddItemDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 StackCount = 1) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	UOWInventoryItemInstance* AddItemDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddItemInstance(UOWInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveItemInstance(UOWInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Inventory)
	TArray<UOWInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	UOWInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	int32 GetTotalItemCountByDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	bool ConsumeItemsByDefinition(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, int32 NumToConsume);

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void BroadcastChangeMessage(const FOWInventoryChangeMessage& Message) const;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
	FOWInventoryChanged OnInventoryChanged;

private:
	UPROPERTY(Replicated)
	FOWInventoryList InventoryList;
};

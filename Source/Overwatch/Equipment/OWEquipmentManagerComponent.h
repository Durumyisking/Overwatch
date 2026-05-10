#pragma once

#include "AbilitySystem/OWAbilitySet.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "OWEquipmentManagerComponent.generated.h"

class UActorComponent;
class UOWAbilitySystemComponent;
class UOWEquipmentDefinition;
class UOWEquipmentInstance;
class UOWEquipmentManagerComponent;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** Pawn에 적용된 단일 장비 항목이다. */
USTRUCT(BlueprintType)
struct FOWAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FString GetDebugString() const;

private:
	friend struct FOWEquipmentList;
	friend class UOWEquipmentManagerComponent;

	UPROPERTY()
	TSubclassOf<UOWEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UOWEquipmentInstance> Instance = nullptr;

	/** 장착 시 부여된 AbilitySet 핸들은 서버에서만 보관하고 해제 시 회수한다. */
	UPROPERTY(NotReplicated)
	FOWAbilitySet_GrantedHandles GrantedHandles;
};

/** 장비 목록의 추가/삭제를 항목 단위로 복제하는 FastArray 컨테이너다. */
USTRUCT(BlueprintType)
struct FOWEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FOWEquipmentList()
		: OwnerComponent(nullptr)
	{}

	FOWEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOWAppliedEquipmentEntry, FOWEquipmentList>(Entries, DeltaParms, *this);
	}

	UOWEquipmentInstance* AddEntry(TSubclassOf<UOWEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(UOWEquipmentInstance* Instance);

private:
	UOWAbilitySystemComponent* GetAbilitySystemComponent() const;

private:
	friend class UOWEquipmentManagerComponent;

	UPROPERTY()
	TArray<FOWAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FOWEquipmentList> : public TStructOpsTypeTraitsBase2<FOWEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};

/** Pawn이 현재 장착 중인 EquipmentInstance 목록을 소유하고 복제한다. */
UCLASS(BlueprintType, Const, meta = (BlueprintSpawnableComponent))
class OVERWATCH_API UOWEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UOWEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Equipment)
	UOWEquipmentInstance* EquipItem(TSubclassOf<UOWEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Equipment)
	void UnequipItem(UOWEquipmentInstance* ItemInstance);

	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Equipment)
	UOWEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UOWEquipmentInstance> InstanceType);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Equipment)
	TArray<UOWEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UOWEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return Cast<T>(GetFirstInstanceOfType(T::StaticClass()));
	}

private:
	UPROPERTY(Replicated)
	FOWEquipmentList EquipmentList;
};

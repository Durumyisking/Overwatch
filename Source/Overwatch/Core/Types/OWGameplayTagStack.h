#pragma once

#include "GameplayTagContainer.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "OWGameplayTagStack.generated.h"

struct FNetDeltaSerializeInfo;

/** GameplayTag 하나와 그 누적 개수를 복제하기 위한 FastArray 항목이다. */
USTRUCT(BlueprintType)
struct FOWGameplayTagStack : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FOWGameplayTagStack()
	{}

	FOWGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{}

	FString GetDebugString() const;

private:
	friend struct FOWGameplayTagStackContainer;

	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 StackCount = 0;
};

/** Inventory item처럼 작은 상태값을 GameplayTag 개수로 표현하고 델타 복제하는 컨테이너다. */
USTRUCT(BlueprintType)
struct FOWGameplayTagStackContainer : public FFastArraySerializer
{
	GENERATED_BODY()

	void AddStack(FGameplayTag InTag, int32 InStackCount);
	void RemoveStack(FGameplayTag InTag, int32 InStackCount);

	int32 GetStackCount(FGameplayTag InTag) const
	{
		return TagToCountMap.FindRef(InTag);
	}

	bool ContainsTag(FGameplayTag InTag) const
	{
		return TagToCountMap.Contains(InTag);
	}

	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOWGameplayTagStack, FOWGameplayTagStackContainer>(Stacks, DeltaParms, *this);
	}

private:
	UPROPERTY()
	TArray<FOWGameplayTagStack> Stacks;

	TMap<FGameplayTag, int32> TagToCountMap;
};

template<>
struct TStructOpsTypeTraits<FOWGameplayTagStackContainer> : public TStructOpsTypeTraitsBase2<FOWGameplayTagStackContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

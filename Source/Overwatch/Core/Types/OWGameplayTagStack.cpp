#include "Core/Types/OWGameplayTagStack.h"

#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWGameplayTagStack)

FString FOWGameplayTagStack::GetDebugString() const
{
	return FString::Printf(TEXT("%sx%d"), *Tag.ToString(), StackCount);
}

void FOWGameplayTagStackContainer::AddStack(FGameplayTag InTag, int32 InStackCount)
{
	if (!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("FOWGameplayTagStackContainer::AddStack received an invalid tag."), ELogVerbosity::Warning);
		return;
	}

	if (InStackCount <= 0)
	{
		return;
	}

	for (FOWGameplayTagStack& Stack : Stacks)
	{
		if (Stack.Tag == InTag)
		{
			const int32 NewCount = Stack.StackCount + InStackCount;
			Stack.StackCount = NewCount;
			TagToCountMap[InTag] = NewCount;
			MarkItemDirty(Stack);
			return;
		}
	}

	FOWGameplayTagStack& NewStack = Stacks.Emplace_GetRef(InTag, InStackCount);
	TagToCountMap.Add(InTag, InStackCount);
	MarkItemDirty(NewStack);
}

void FOWGameplayTagStackContainer::RemoveStack(FGameplayTag InTag, int32 InStackCount)
{
	if (!InTag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("FOWGameplayTagStackContainer::RemoveStack received an invalid tag."), ELogVerbosity::Warning);
		return;
	}

	if (InStackCount <= 0)
	{
		return;
	}

	for (auto StackIt = Stacks.CreateIterator(); StackIt; ++StackIt)
	{
		FOWGameplayTagStack& Stack = *StackIt;
		if (Stack.Tag != InTag)
		{
			continue;
		}

		if (Stack.StackCount <= InStackCount)
		{
			StackIt.RemoveCurrent();
			TagToCountMap.Remove(InTag);
			MarkArrayDirty();
		}
		else
		{
			const int32 NewCount = Stack.StackCount - InStackCount;
			Stack.StackCount = NewCount;
			TagToCountMap[InTag] = NewCount;
			MarkItemDirty(Stack);
		}

		return;
	}
}

void FOWGameplayTagStackContainer::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	for (int32 Index : RemovedIndices)
	{
		TagToCountMap.Remove(Stacks[Index].Tag);
	}
}

void FOWGameplayTagStackContainer::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	for (int32 Index : AddedIndices)
	{
		const FOWGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap.Add(Stack.Tag, Stack.StackCount);
	}
}

void FOWGameplayTagStackContainer::PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize)
{
	for (int32 Index : ChangedIndices)
	{
		const FOWGameplayTagStack& Stack = Stacks[Index];
		TagToCountMap[Stack.Tag] = Stack.StackCount;
	}
}

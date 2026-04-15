#include "OWGameplayTags.h"

#include "GameplayTagsManager.h"

bool FOWGameplayTags::bIsInitialized = false;

namespace
{
void AppendLeafTagsRecursive(const UGameplayTagsManager& InManager, const FGameplayTag& InCurrentTag, TArray<FGameplayTag>& OutLeafTags)
{
	const FGameplayTagContainer Children = InManager.RequestGameplayTagChildren(InCurrentTag);
	if (Children.IsEmpty())
	{
		OutLeafTags.Add(InCurrentTag);
		return;
	}

	TArray<FGameplayTag> ChildTags;
	Children.GetGameplayTagArray(ChildTags);

	for (const FGameplayTag& ChildTag : ChildTags)
	{
		AppendLeafTagsRecursive(InManager, ChildTag, OutLeafTags);
	}
}
}

void FOWGameplayTags::InitializeNativeTags()
{
	if (bIsInitialized)
	{
		return;
	}

	UGameplayTagsManager::Get();
	bIsInitialized = true;
}

void FOWGameplayTags::GetGameplayTagListAtDepth(const FGameplayTag& InRootTag, int32 InDepth, TArray<FGameplayTag>& OutTags, bool bSortLexicographically)
{
	OutTags.Reset();

	if (!InRootTag.IsValid() || InDepth < 0)
	{
		return;
	}

	if (InDepth == 0)
	{
		OutTags.Add(InRootTag);
		return;
	}

	const int32 RootSegmentCount = GetTagSegmentCount(InRootTag);
	if (RootSegmentCount <= 0)
	{
		return;
	}

	const FGameplayTagContainer Descendants = UGameplayTagsManager::Get().RequestGameplayTagChildren(InRootTag);
	TArray<FGameplayTag> DescendantTags;
	Descendants.GetGameplayTagArray(DescendantTags);

	for (const FGameplayTag& DescendantTag : DescendantTags)
	{
		const int32 RelativeDepth = GetTagSegmentCount(DescendantTag) - RootSegmentCount;
		if (RelativeDepth == InDepth)
		{
			OutTags.Add(DescendantTag);
		}
	}

	if (bSortLexicographically)
	{
		OutTags.Sort([](const FGameplayTag& InLeft, const FGameplayTag& InRight) {
			return InLeft.ToString() < InRight.ToString();
		});
	}
}

FString FOWGameplayTags::GetGameplayTagLeafString(const FGameplayTag& InTag)
{
	const FString TagString = InTag.ToString();
	TArray<FString> Segments;
	TagString.ParseIntoArray(Segments, TEXT("."), true);

	return Segments.Num() > 0 ? Segments.Last() : TagString;
}

void FOWGameplayTags::GetLeafTagsUnder(const FGameplayTag& InRootTag, TArray<FGameplayTag>& OutLeafTags, bool bSortLexicographically)
{
	OutLeafTags.Reset();

	if (!InRootTag.IsValid())
	{
		return;
	}

	const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	const FGameplayTagContainer Children = Manager.RequestGameplayTagChildren(InRootTag);
	if (Children.IsEmpty())
	{
		return;
	}

	TArray<FGameplayTag> ChildTags;
	Children.GetGameplayTagArray(ChildTags);

	for (const FGameplayTag& ChildTag : ChildTags)
	{
		AppendLeafTagsRecursive(Manager, ChildTag, OutLeafTags);
	}

	if (bSortLexicographically)
	{
		OutLeafTags.Sort([](const FGameplayTag& InLeft, const FGameplayTag& InRight) {
			return InLeft.ToString() < InRight.ToString();
		});
	}
}

bool FOWGameplayTags::IsLeafTag(const FGameplayTag& InTag)
{
	if (!InTag.IsValid())
	{
		return false;
	}

	return UGameplayTagsManager::Get().RequestGameplayTagChildren(InTag).IsEmpty();
}

int32 FOWGameplayTags::GetTagSegmentCount(const FGameplayTag& InTag)
{
	if (!InTag.IsValid())
	{
		return 0;
	}

	const FString TagString = InTag.ToString();
	if (TagString.IsEmpty())
	{
		return 0;
	}

	int32 SegmentCount = 1;
	for (TCHAR Character : TagString)
	{
		if (Character == TEXT('.'))
		{
			SegmentCount += 1;
		}
	}

	return SegmentCount;
}

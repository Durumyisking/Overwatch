#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct OVERWATCH_API FOWGameplayTags
{
	static void InitializeNativeTags();
	static void GetGameplayTagListAtDepth(const FGameplayTag& InRootTag, int32 InDepth, TArray<FGameplayTag>& OutTags, bool bSortLexicographically = false);
	static FString GetGameplayTagLeafString(const FGameplayTag& InTag);
	static void GetLeafTagsUnder(const FGameplayTag& InRootTag, TArray<FGameplayTag>& OutLeafTags, bool bSortLexicographically = true);
	static bool IsLeafTag(const FGameplayTag& InTag);

private:
	static int32 GetTagSegmentCount(const FGameplayTag& InTag);
	static bool bIsInitialized;
};

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct OVERWATCH_API FOWGameplayTags
{
	static const FOWGameplayTags& Get();
	static void InitializeNativeTags();
	void AddAllTags();
	static const FGameplayTag& GetInputTagCrouch();
	static const FGameplayTag& GetInputTagJump();
	static const FGameplayTag& GetInputTagMove();
	static const FGameplayTag& GetInputTagLook();
	static const FGameplayTag& GetInputTagLookMouse();
	static const FGameplayTag& GetInputTagShoot();
	static const FGameplayTag& GetInputTagReload();
	static const FGameplayTag& GetInputTagAbilitySkill1();
	static const FGameplayTag& GetInputTagAbilitySkill2();
	static const FGameplayTag& GetInputTagAbilitySkill3();
	static const FGameplayTag& GetInputTagAbilityUltimate();
	static void GetGameplayTagListAtDepth(const FGameplayTag& InRootTag, int32 InDepth, TArray<FGameplayTag>& OutTags, bool bSortLexicographically = false);
	static FString GetGameplayTagLeafString(const FGameplayTag& InTag);
	static void GetLeafTagsUnder(const FGameplayTag& InRootTag, TArray<FGameplayTag>& OutLeafTags, bool bSortLexicographically = true);
	static bool IsLeafTag(const FGameplayTag& InTag);

	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;

private:
	static void AddTag(FGameplayTag& OutTag, const ANSICHAR* InTagName, const ANSICHAR* InTagComment);
	static int32 GetTagSegmentCount(const FGameplayTag& InTag);
	static bool bIsInitialized;
	static FOWGameplayTags GameplayTags;
	static FGameplayTag InputTag_Crouch;
	static FGameplayTag InputTag_Jump;
	static FGameplayTag InputTag_Move;
	static FGameplayTag InputTag_Look;
	static FGameplayTag InputTag_Look_Mouse;
	static FGameplayTag InputTag_Shoot;
	static FGameplayTag InputTag_Reload;
	static FGameplayTag InputTag_Ability_Skill1;
	static FGameplayTag InputTag_Ability_Skill2;
	static FGameplayTag InputTag_Ability_Skill3;
	static FGameplayTag InputTag_Ability_Ultimate;
};

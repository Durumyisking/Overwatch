#include "OWGameplayTags.h"

#include "GameplayTagsManager.h"

bool FOWGameplayTags::bIsInitialized = false;
FGameplayTag FOWGameplayTags::InputTag_Crouch;
FGameplayTag FOWGameplayTags::InputTag_Jump;
FGameplayTag FOWGameplayTags::InputTag_Move;
FGameplayTag FOWGameplayTags::InputTag_Look;
FGameplayTag FOWGameplayTags::InputTag_Look_Mouse;
FGameplayTag FOWGameplayTags::InputTag_Shoot;
FGameplayTag FOWGameplayTags::InputTag_Reload;
FGameplayTag FOWGameplayTags::InputTag_Ability_Skill1;
FGameplayTag FOWGameplayTags::InputTag_Ability_Skill2;
FGameplayTag FOWGameplayTags::InputTag_Ability_Skill3;
FGameplayTag FOWGameplayTags::InputTag_Ability_Ultimate;

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
	AddTag(InputTag_Crouch, "InputTag.Crouch", "Input for crouch.");
	AddTag(InputTag_Jump, "InputTag.Jump", "Input for jump.");
	AddTag(InputTag_Move, "InputTag.Move", "Input for movement.");
	AddTag(InputTag_Look, "InputTag.Look", "Input for gamepad or generic look.");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "Input for mouse look.");
	AddTag(InputTag_Shoot, "InputTag.Combat.Shoot", "Input for primary fire.");
	AddTag(InputTag_Reload, "InputTag.Combat.Reload", "Input for reload.");
	AddTag(InputTag_Ability_Skill1, "InputTag.Ability.Skill1", "Input for hero ability slot 1.");
	AddTag(InputTag_Ability_Skill2, "InputTag.Ability.Skill2", "Input for hero ability slot 2.");
	AddTag(InputTag_Ability_Skill3, "InputTag.Ability.Skill3", "Input for hero ability slot 3.");
	AddTag(InputTag_Ability_Ultimate, "InputTag.Ability.Ultimate", "Input for ultimate ability.");
	bIsInitialized = true;
}

const FGameplayTag& FOWGameplayTags::GetInputTagCrouch()
{
	return InputTag_Crouch;
}

const FGameplayTag& FOWGameplayTags::GetInputTagJump()
{
	return InputTag_Jump;
}

const FGameplayTag& FOWGameplayTags::GetInputTagMove()
{
	return InputTag_Move;
}

const FGameplayTag& FOWGameplayTags::GetInputTagLook()
{
	return InputTag_Look;
}

const FGameplayTag& FOWGameplayTags::GetInputTagLookMouse()
{
	return InputTag_Look_Mouse;
}

const FGameplayTag& FOWGameplayTags::GetInputTagShoot()
{
	return InputTag_Shoot;
}

const FGameplayTag& FOWGameplayTags::GetInputTagReload()
{
	return InputTag_Reload;
}

const FGameplayTag& FOWGameplayTags::GetInputTagAbilitySkill1()
{
	return InputTag_Ability_Skill1;
}

const FGameplayTag& FOWGameplayTags::GetInputTagAbilitySkill2()
{
	return InputTag_Ability_Skill2;
}

const FGameplayTag& FOWGameplayTags::GetInputTagAbilitySkill3()
{
	return InputTag_Ability_Skill3;
}

const FGameplayTag& FOWGameplayTags::GetInputTagAbilityUltimate()
{
	return InputTag_Ability_Ultimate;
}

void FOWGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* InTagName, const ANSICHAR* InTagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(InTagName), FString(TEXT("(Native) ")) + FString(InTagComment));
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

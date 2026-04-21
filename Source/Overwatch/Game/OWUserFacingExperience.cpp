#include "Game/OWUserFacingExperience.h"

#if __has_include("CommonSessionSubsystem.h")
#include "CommonSessionSubsystem.h"
#endif

UObject* UOWUserFacingExperience::CreateHostingRequest() const
{
#if __has_include("CommonSessionSubsystem.h")
	const FString ExperienceName = ExperienceID.PrimaryAssetName.ToString();

	UCommonSession_HostSessionRequest* Result = NewObject<UCommonSession_HostSessionRequest>();
	Result->MapID = MapID;
	Result->ExtraArgs.Add(TEXT("Experience"), ExperienceName);
	return Result;
#else
	return nullptr;
#endif
}

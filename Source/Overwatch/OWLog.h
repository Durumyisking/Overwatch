#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

OVERWATCH_API DECLARE_LOG_CATEGORY_EXTERN(LogOW, Log, All);
OVERWATCH_API DECLARE_LOG_CATEGORY_EXTERN(LogOWCore, Log, All);
OVERWATCH_API DECLARE_LOG_CATEGORY_EXTERN(LogOWGame, Log, All);
OVERWATCH_API DECLARE_LOG_CATEGORY_EXTERN(LogOWUI, Log, All);
OVERWATCH_API DECLARE_LOG_CATEGORY_EXTERN(LogOWAbilitySystem, Log, All);
OVERWATCH_API DECLARE_LOG_CATEGORY_EXTERN(LogOWNetwork, Log, All);

class UObject;
class UWorld;
class AActor;

struct OVERWATCH_API FOWLog
{
	static const TCHAR* GetNetModeString(ENetMode InNetMode);
	static const TCHAR* GetRoleString(ENetRole InRole);
	static ENetMode GetNetMode(const UObject* InWorldContextObject);
	static FColor GetDisplayColor(ELogVerbosity::Type InVerbosity);
	static FString BuildWorldContextString(const UObject* InWorldContextObject);
	static FString BuildObjectContextString(const UObject* InObject);
	static FString BuildActorContextString(const AActor* InActor);
	static void Display(const FString& InMessage, FColor InColor = FColor::Cyan, float InDuration = 5.0f, uint64 InKey = static_cast<uint64>(-1), bool bInNewerOnTop = true);
	static void DisplayWorld(const UObject* InWorldContextObject, const FString& InMessage, FColor InColor = FColor::Cyan, float InDuration = 5.0f, uint64 InKey = static_cast<uint64>(-1), bool bInNewerOnTop = true);
	static void DisplayObject(const UObject* InObject, const FString& InMessage, FColor InColor = FColor::Cyan, float InDuration = 5.0f, uint64 InKey = static_cast<uint64>(-1), bool bInNewerOnTop = true);
	static void DisplayActor(const AActor* InActor, const FString& InMessage, FColor InColor = FColor::Cyan, float InDuration = 5.0f, uint64 InKey = static_cast<uint64>(-1), bool bInNewerOnTop = true);
};

#define OW_LOG(InCategory, InVerbosity, Format, ...) UE_LOG(InCategory, InVerbosity, TEXT("[%s] " Format), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__)
#define OW_CLOG(Condition, InCategory, InVerbosity, Format, ...) UE_CLOG(Condition, InCategory, InVerbosity, TEXT("[%s] " Format), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__)
#define OW_WARN(InCategory, Format, ...) OW_LOG(InCategory, Warning, Format, ##__VA_ARGS__)
#define OW_ERROR(InCategory, Format, ...) OW_LOG(InCategory, Error, Format, ##__VA_ARGS__)
#define OW_NET_LOG(InCategory, InVerbosity, InWorldContextObject, Format, ...) UE_LOG(InCategory, InVerbosity, TEXT("[%s][%s] " Format), ANSI_TO_TCHAR(__FUNCTION__), *FOWLog::BuildWorldContextString(InWorldContextObject), ##__VA_ARGS__)
#define OW_OBJ_LOG(InCategory, InVerbosity, InObject, Format, ...) UE_LOG(InCategory, InVerbosity, TEXT("[%s][%s] " Format), ANSI_TO_TCHAR(__FUNCTION__), *FOWLog::BuildObjectContextString(InObject), ##__VA_ARGS__)
#define OW_ACTOR_LOG(InCategory, InVerbosity, InActor, Format, ...) UE_LOG(InCategory, InVerbosity, TEXT("[%s][%s] " Format), ANSI_TO_TCHAR(__FUNCTION__), *FOWLog::BuildActorContextString(InActor), ##__VA_ARGS__)
#define OW_DISPLAY(Format, ...) FOWLog::Display(FString::Printf(TEXT("[%s] " Format), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__))
#define OW_DISPLAY_LOG(InCategory, InVerbosity, Format, ...) do { OW_LOG(InCategory, InVerbosity, Format, ##__VA_ARGS__); FOWLog::Display(FString::Printf(TEXT("[%s][%s] " Format), TEXT(#InCategory), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__), FOWLog::GetDisplayColor(InVerbosity)); } while(false)
#define OW_DISPLAY_WARN(InCategory, Format, ...) OW_DISPLAY_LOG(InCategory, Warning, Format, ##__VA_ARGS__)
#define OW_DISPLAY_ERROR(InCategory, Format, ...) OW_DISPLAY_LOG(InCategory, Error, Format, ##__VA_ARGS__)
#define OW_DISPLAY_NET(InCategory, InVerbosity, InWorldContextObject, Format, ...) do { OW_NET_LOG(InCategory, InVerbosity, InWorldContextObject, Format, ##__VA_ARGS__); FOWLog::DisplayWorld(InWorldContextObject, FString::Printf(TEXT("[%s][%s] " Format), TEXT(#InCategory), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__), FOWLog::GetDisplayColor(InVerbosity)); } while(false)
#define OW_DISPLAY_OBJ(InCategory, InVerbosity, InObject, Format, ...) do { OW_OBJ_LOG(InCategory, InVerbosity, InObject, Format, ##__VA_ARGS__); FOWLog::DisplayObject(InObject, FString::Printf(TEXT("[%s][%s] " Format), TEXT(#InCategory), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__), FOWLog::GetDisplayColor(InVerbosity)); } while(false)
#define OW_DISPLAY_ACTOR(InCategory, InVerbosity, InActor, Format, ...) do { OW_ACTOR_LOG(InCategory, InVerbosity, InActor, Format, ##__VA_ARGS__); FOWLog::DisplayActor(InActor, FString::Printf(TEXT("[%s][%s] " Format), TEXT(#InCategory), ANSI_TO_TCHAR(__FUNCTION__), ##__VA_ARGS__), FOWLog::GetDisplayColor(InVerbosity)); } while(false)

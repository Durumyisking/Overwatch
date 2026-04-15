#include "OWLog.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "UObject/Object.h"

DEFINE_LOG_CATEGORY(LogOW);
DEFINE_LOG_CATEGORY(LogOWCore);
DEFINE_LOG_CATEGORY(LogOWGame);
DEFINE_LOG_CATEGORY(LogOWUI);
DEFINE_LOG_CATEGORY(LogOWAbilitySystem);
DEFINE_LOG_CATEGORY(LogOWNetwork);

const TCHAR* FOWLog::GetNetModeString(ENetMode InNetMode)
{
	switch (InNetMode)
	{
	case NM_Standalone:
		return TEXT("Standalone");
	case NM_DedicatedServer:
		return TEXT("DedicatedServer");
	case NM_ListenServer:
		return TEXT("ListenServer");
	case NM_Client:
		return TEXT("Client");
	default:
		return TEXT("Unknown");
	}
}

const TCHAR* FOWLog::GetRoleString(ENetRole InRole)
{
	switch (InRole)
	{
	case ROLE_None:
		return TEXT("None");
	case ROLE_SimulatedProxy:
		return TEXT("SimulatedProxy");
	case ROLE_AutonomousProxy:
		return TEXT("AutonomousProxy");
	case ROLE_Authority:
		return TEXT("Authority");
	default:
		return TEXT("Unknown");
	}
}

ENetMode FOWLog::GetNetMode(const UObject* InWorldContextObject)
{
	if (!InWorldContextObject)
	{
		return NM_Standalone;
	}

	const UWorld* World = InWorldContextObject->GetWorld();
	if (!World)
	{
		return NM_Standalone;
	}

	return World->GetNetMode();
}

FColor FOWLog::GetDisplayColor(ELogVerbosity::Type InVerbosity)
{
	switch (InVerbosity)
	{
	case ELogVerbosity::Fatal:
	case ELogVerbosity::Error:
		return FColor::Red;
	case ELogVerbosity::Warning:
		return FColor::Yellow;
	case ELogVerbosity::Display:
		return FColor::Green;
	case ELogVerbosity::Verbose:
	case ELogVerbosity::VeryVerbose:
		return FColor::Silver;
	default:
		return FColor::Cyan;
	}
}

FString FOWLog::BuildWorldContextString(const UObject* InWorldContextObject)
{
	if (!InWorldContextObject)
	{
		return TEXT("World=None");
	}

	const UWorld* World = InWorldContextObject->GetWorld();
	if (!World)
	{
		return FString::Printf(TEXT("World=None Object=%s"), *GetNameSafe(InWorldContextObject));
	}

	return FString::Printf(TEXT("World=%s NetMode=%s"), *GetNameSafe(World), GetNetModeString(World->GetNetMode()));
}

FString FOWLog::BuildObjectContextString(const UObject* InObject)
{
	if (!InObject)
	{
		return TEXT("Object=None");
	}

	const UWorld* World = InObject->GetWorld();
	if (!World)
	{
		return FString::Printf(TEXT("Object=%s Path=%s"), *GetNameSafe(InObject), *InObject->GetPathName());
	}

	return FString::Printf(TEXT("Object=%s Path=%s NetMode=%s"), *GetNameSafe(InObject), *InObject->GetPathName(), GetNetModeString(World->GetNetMode()));
}

FString FOWLog::BuildActorContextString(const AActor* InActor)
{
	if (!InActor)
	{
		return TEXT("Actor=None");
	}

	const UWorld* World = InActor->GetWorld();
	return FString::Printf(
		TEXT("Actor=%s NetMode=%s LocalRole=%s RemoteRole=%s"),
		*GetNameSafe(InActor),
		GetNetModeString(World ? World->GetNetMode() : NM_Standalone),
		GetRoleString(InActor->GetLocalRole()),
		GetRoleString(InActor->GetRemoteRole()));
}

void FOWLog::Display(const FString& InMessage, FColor InColor, float InDuration, uint64 InKey, bool bInNewerOnTop)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(InKey, InDuration, InColor, InMessage, bInNewerOnTop);
}

void FOWLog::DisplayWorld(const UObject* InWorldContextObject, const FString& InMessage, FColor InColor, float InDuration, uint64 InKey, bool bInNewerOnTop)
{
	Display(FString::Printf(TEXT("[%s] %s"), *BuildWorldContextString(InWorldContextObject), *InMessage), InColor, InDuration, InKey, bInNewerOnTop);
}

void FOWLog::DisplayObject(const UObject* InObject, const FString& InMessage, FColor InColor, float InDuration, uint64 InKey, bool bInNewerOnTop)
{
	Display(FString::Printf(TEXT("[%s] %s"), *BuildObjectContextString(InObject), *InMessage), InColor, InDuration, InKey, bInNewerOnTop);
}

void FOWLog::DisplayActor(const AActor* InActor, const FString& InMessage, FColor InColor, float InDuration, uint64 InKey, bool bInNewerOnTop)
{
	Display(FString::Printf(TEXT("[%s] %s"), *BuildActorContextString(InActor), *InMessage), InColor, InDuration, InKey, bInNewerOnTop);
}

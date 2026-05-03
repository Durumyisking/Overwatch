// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CommonSessionSubsystem.generated.h"

/**
 * UCommonSession_HostSessionRequest를 만들어 CommonSessionSubsystem 전달하면, CommonSessionSubsystem에서 MapID와 CmdArgs로 최종적인 CmdArgs를 만들어 맵 로딩 진행
 */
UCLASS(BlueprintType)
class COMMONUSER_API UCommonSession_HostSessionRequest : public UObject
{
	GENERATED_BODY()
public:
	/** MapID -> Text 변환 */
	FString GetMapName() const;

	/** ServerTravel에 전달할 최종 URL을 생성한다 */
	FString ConstructTravelURL() const;

	/** 준비할 MapID (맵 경로) */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	FPrimaryAssetId MapID;

	/** 전달할 CmdArgs (Experience 이름을 전달함) -> 곧 UserFacingExperienceDefinition에서 보게 될 것임 */
	UPROPERTY(BlueprintReadWrite, Category = Session)
	TMap<FString, FString> ExtraArgs;
};
UCLASS(MinimalAPI, BlueprintType)
class UCommonSession_SearchResult : public UObject
{
	GENERATED_BODY()

public:
	///** Returns an internal description of the session, not meant to be human readable */
	//UFUNCTION(BlueprintCallable, Category = Session)
	//COMMONUSER_API FString GetDescription() const;

	///** Gets an arbitrary string setting, bFoundValue will be false if the setting does not exist */
	//UFUNCTION(BlueprintPure, Category = Sessions)
	//COMMONUSER_API void GetStringSetting(FName Key, FString& Value, bool& bFoundValue) const;

	///** Gets an arbitrary integer setting, bFoundValue will be false if the setting does not exist */
	//UFUNCTION(BlueprintPure, Category = Sessions)
	//COMMONUSER_API void GetIntSetting(FName Key, int32& Value, bool& bFoundValue) const;

	///** The number of private connections that are available */
	//UFUNCTION(BlueprintPure, Category = Sessions)
	//COMMONUSER_API int32 GetNumOpenPrivateConnections() const;

	///** The number of publicly available connections that are available */
	//UFUNCTION(BlueprintPure, Category = Sessions)
	//COMMONUSER_API int32 GetNumOpenPublicConnections() const;

	///** The maximum number of publicly available connections that could be available, including already filled connections */
	//UFUNCTION(BlueprintPure, Category = Sessions)
	//COMMONUSER_API int32 GetMaxPublicConnections() const;

	///** Ping to the search result, MAX_QUERY_PING is unreachable */
	//UFUNCTION(BlueprintPure, Category = Sessions)
	//COMMONUSER_API int32 GetPingInMs() const;

public:
	/** Pointer to the platform-specific implementation */
//#if COMMONUSER_OSSV1
	//FOnlineSessionSearchResult Result;
//#else
	TSharedPtr < const UE::Online::OnlineIdHandleTags::FLobby > Lobby;

	UE::Online::FOnlineSessionId SessionID;
//#endif // COMMONUSER_OSSV1

};


/**
 * UCommonSessionSubsystem은 HakGame에서 사용되야 하기 때문에, Module Export를 해줘야하고 그래서 COMMONUSER_API를 추가해줘야 한다!
 * - 여러분들이 {ModuleName}_API의 추가는 ***다른 모듈에서 사용할 경우, 추가해주면 된다***
 */
UCLASS()
class COMMONUSER_API UCommonSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	UCommonSessionSubsystem() {}

	UFUNCTION(BlueprintCallable, Category = Session)
	void HostSession(APlayerController* HostingPlayer, UCommonSession_HostSessionRequest* Request);
	//UFUNCTION(BlueprintCallable, Category = Session)
	//virtual void JoinSession(APlayerController* JoiningPlayer, UCommonSession_SearchResult* Request);

	/**
	 * member variables
	 */
	 /** PendingTravelURL은 흔히 맵의 경로로 생각하면 된다 */
	FString PendingTravelURL;
};

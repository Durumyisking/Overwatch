// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "Cosmetics/OWCosmeticAnimationTypes.h"
#include "OWCharacterPartTypes.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "OWPawnComponent_CharacterParts.generated.h"

class UOWPawnComponent_CharacterParts;
namespace EEndPlayReason { enum Type : int; }
struct FGameplayTag;
struct FOWCharacterPartList;

class AActor;
class UChildActorComponent;
class UObject;
class USceneComponent;
class USkeletalMeshComponent;
struct FFrame;
struct FNetDeltaSerializeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOWSpawnedCharacterPartsChanged, UOWPawnComponent_CharacterParts*, ComponentWithChangedParts);

//////////////////////////////////////////////////////////////////////

// 적용된 단일 캐릭터 파츠
USTRUCT()
struct FOWAppliedCharacterPartEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	// 서버 리플리케이트를 위해 Entry + List 형태의 FastArray 패턴을 사용합니다.
	// 일반 TArray를 복제하면 배열 전체 상태 중심으로 전달되기 때문에,
	// 클라이언트에서 각 항목이 추가/삭제/변경됐을 때의 생명주기 처리를 세밀하게 연결하기 어렵습니다.
	//
	// 그래서 배열의 각 항목은 FFastArraySerializerItem을 상속하고,
	// 배열을 소유하는 List는 FFastArraySerializer를 상속합니다.
	//
	// Entry는 FastArray 내부 복제 ID/변경 키를 통해 항목 단위 변경 추적 대상이 되고,
	// List는 NetDeltaSerialize에서 FastArrayDeltaSerialize를 호출해 변경된 Entry만 델타 복제합니다.
	//
	// 클라이언트는 PreReplicatedRemove / PostReplicatedAdd / PostReplicatedChange 콜백을 받아
	// 어떤 Entry가 제거/추가/변경됐는지 알고, 그에 맞춰 Cosmetic Actor를 제거/생성/재생성합니다.
	//
	// PartHandle은 FastArray 복제 식별자가 아니라,
	// 서버 코드가 AddCharacterPart로 추가한 파츠를 나중에 RemoveCharacterPart로 제거하기 위한 로컬 핸들입니다.

	FOWAppliedCharacterPartEntry()
	{}

	FString GetDebugString() const;

private:
	friend FOWCharacterPartList;
	friend UOWPawnComponent_CharacterParts;

private:
	// 이 항목이 나타내는 캐릭터 파츠
	UPROPERTY()
	FOWCharacterPart Part;

	// 사용자에게 반환한 핸들 인덱스입니다(서버 전용).
	UPROPERTY(NotReplicated)
	int32 PartHandle = INDEX_NONE;

	// 스폰된 액터 인스턴스입니다(클라이언트 전용).
	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

//////////////////////////////////////////////////////////////////////

// 적용된 캐릭터 파츠의 복제 목록
USTRUCT(BlueprintType)
struct FOWCharacterPartList : public FFastArraySerializer
{
	GENERATED_BODY()

	// Entry 전체를 관리하면서, 어떤 칸이 추가/삭제/변경됐는지 Unreal 복제 시스템에 알려주는 직렬화 관리자

	FOWCharacterPartList()
		: OwnerComponent(nullptr)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FOWAppliedCharacterPartEntry, FOWCharacterPartList>(Entries, DeltaParms, *this);
	}

	FOWCharacterPartHandle AddEntry(FOWCharacterPart NewPart);
	void RemoveEntry(FOWCharacterPartHandle Handle);
	void ClearAllEntries(bool bBroadcastChangeDelegate);

	FGameplayTagContainer CollectCombinedTags() const;

	void SetOwnerComponent(UOWPawnComponent_CharacterParts* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}
	
private:
	friend UOWPawnComponent_CharacterParts;

	bool SpawnActorForEntry(FOWAppliedCharacterPartEntry& Entry);
	bool DestroyActorForEntry(FOWAppliedCharacterPartEntry& Entry);

private:
	// 장비 항목의 복제 목록
	UPROPERTY()
	TArray<FOWAppliedCharacterPartEntry> Entries;

	// 이 목록을 포함하는 컴포넌트
	UPROPERTY(NotReplicated)
	TObjectPtr<UOWPawnComponent_CharacterParts> OwnerComponent;

	// 핸들 생성을 위한 증가 카운터
	int32 PartHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<FOWCharacterPartList> : public TStructOpsTypeTraitsBase2<FOWCharacterPartList>
{
	enum { WithNetDeltaSerializer = true };
};

//////////////////////////////////////////////////////////////////////

// 모든 클라이언트에서 소유 Pawn에 부착되는 코스메틱 액터 스폰을 처리하는 컴포넌트
UCLASS(meta=(BlueprintSpawnableComponent))
class UOWPawnComponent_CharacterParts : public UPawnComponent
{
	GENERATED_BODY()

public:
	UOWPawnComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~End of UActorComponent interface

	// 이 커스터마이징 컴포넌트를 소유한 액터에 캐릭터 파츠를 추가합니다. 서버 권한에서만 호출해야 합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	FOWCharacterPartHandle AddCharacterPart(const FOWCharacterPart& NewPart);

	// 이 커스터마이징 컴포넌트를 소유한 액터에서 이전에 추가한 캐릭터 파츠를 제거합니다. 서버 권한에서만 호출해야 합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(FOWCharacterPartHandle Handle);

	// 추가된 모든 캐릭터 파츠를 제거합니다. 서버 권한에서만 호출해야 합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// 이 컴포넌트에서 스폰한 모든 캐릭터 파츠 목록을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	TArray<AActor*> GetCharacterPartActors() const;

	// 부모 액터가 ACharacter에서 파생되었으면 메시 컴포넌트를 반환하고, 아니면 nullptr를 반환합니다.
	USkeletalMeshComponent* GetParentMeshComponent() const;

	// 스폰된 액터를 부착할 씬 컴포넌트를 반환합니다.
	// 부모 액터가 ACharacter에서 파생되었으면 메시 컴포넌트를 사용하고, 아니면 루트 컴포넌트를 사용합니다.
	USceneComponent* GetSceneComponentToAttachTo() const;

	// 부착된 캐릭터 파츠의 통합 게임플레이 태그 집합을 반환합니다. 지정한 루트로 시작하는 태그만 선택적으로 필터링할 수 있습니다.
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

public:
	// 스폰된 캐릭터 파츠 목록이 변경될 때 호출되는 델리게이트
	UPROPERTY(BlueprintAssignable, Category=Cosmetics, BlueprintCallable)
	FOWSpawnedCharacterPartsChanged OnCharacterPartsChanged;

private:
	// 캐릭터 파츠 목록
	UPROPERTY(Replicated, Transient)
	FOWCharacterPartList CharacterPartList;

	// 캐릭터 파츠 코스메틱 태그를 기준으로 애니메이션을 재생할 바디 스타일 메시를 고르는 규칙
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	FOWAnimBodyStyleSelectionSet BodyMeshes;
};

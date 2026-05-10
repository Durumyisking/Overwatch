// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Templates/SubclassOf.h"

#include "OWCosmeticAnimationTypes.generated.h"

class UAnimInstance;
class UPhysicsAsset;
class USkeletalMesh;

//////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FOWAnimLayerSelectionEntry
{
	GENERATED_BODY()

	// 태그가 일치할 때 적용할 레이어
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> Layer;

	// 필요한 코스메틱 태그입니다. 모두 있어야 일치로 간주합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FOWAnimLayerSelectionSet
{
	GENERATED_BODY()
		
	// 적용할 레이어 규칙 목록입니다. 처음 일치한 규칙을 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Layer))
	TArray<FOWAnimLayerSelectionEntry> LayerRules;

	// LayerRules 중 일치하는 항목이 없을 때 사용할 레이어
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> DefaultLayer;

	// 규칙에 따라 가장 적합한 레이어를 선택합니다.
	TSubclassOf<UAnimInstance> SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const;
};

//////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FOWAnimBodyStyleSelectionEntry
{
	GENERATED_BODY()

	// 태그가 일치할 때 적용할 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> Mesh = nullptr;

	// 필요한 코스메틱 태그입니다. 모두 있어야 일치로 간주합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Cosmetic"))
	FGameplayTagContainer RequiredTags;
};

// 장착된 코스메틱 파츠들의 GameplayTag 조합을 보고 Pawn 본체의 USkeletalMesh를 고르는 규칙 테이블입니다.
USTRUCT(BlueprintType)
struct FOWAnimBodyStyleSelectionSet
{
	GENERATED_BODY()
		
	// 적용할 메시 규칙 목록입니다. 처음 일치한 규칙을 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty=Mesh))
	TArray<FOWAnimBodyStyleSelectionEntry> MeshRules;

	// MeshRules 중 일치하는 항목이 없을 때 사용할 기본 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> DefaultMesh = nullptr;

	// 설정되어 있으면 이 물리 에셋을 항상 사용합니다.
	UPROPERTY(EditAnywhere)
	TObjectPtr<UPhysicsAsset> ForcedPhysicsAsset = nullptr;

	// 규칙에 따라 가장 적합한 바디 스타일 스켈레탈 메시를 선택합니다.
	USkeletalMesh* SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const;
};

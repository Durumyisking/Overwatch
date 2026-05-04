// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "OWCharacterPartTypes.generated.h"

class UChildActorComponent;
class UOWPawnComponent_CharacterParts;
struct FOWCharacterPartList;

//////////////////////////////////////////////////////////////////////

// 스폰된 파츠 액터의 충돌을 어떻게 구성할지 지정합니다.
UENUM()
enum class ECharacterCustomizationCollisionMode : uint8
{
	// 스폰된 캐릭터 파츠의 충돌을 비활성화합니다.
	NoCollision,

	// 캐릭터 파츠의 충돌 설정을 그대로 둡니다.
	UseCollisionFromCharacterPart
};

//////////////////////////////////////////////////////////////////////

// 캐릭터 파츠 항목을 추가할 때 생성되는 핸들입니다. 나중에 제거할 때 사용할 수 있습니다.
USTRUCT(BlueprintType)
struct FOWCharacterPartHandle
{
	GENERATED_BODY()

	void Reset()
	{
		PartHandle = INDEX_NONE;
	}

	bool IsValid() const
	{
		return PartHandle != INDEX_NONE;
	}

private:
	UPROPERTY()
	int32 PartHandle = INDEX_NONE;

	friend FOWCharacterPartList;
};

//////////////////////////////////////////////////////////////////////
// 캐릭터 파츠 요청

USTRUCT(BlueprintType)
struct FOWCharacterPart
{
	GENERATED_BODY()

	// 스폰할 파츠
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> PartClass;

	// 파츠를 부착할 소켓입니다. 없을 수도 있습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SocketName;

	// 파츠의 Primitive 컴포넌트 충돌 처리 방식
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECharacterCustomizationCollisionMode CollisionMode = ECharacterCustomizationCollisionMode::NoCollision;

	// 충돌 모드를 무시하고 다른 파츠와 비교합니다.
	static bool AreEquivalentParts(const FOWCharacterPart& A, const FOWCharacterPart& B)
	{
		return (A.PartClass == B.PartClass) && (A.SocketName == B.SocketName);
	}
};

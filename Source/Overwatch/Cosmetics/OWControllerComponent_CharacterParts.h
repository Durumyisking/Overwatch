// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"
#include "OWCharacterPartTypes.h"

#include "OWControllerComponent_CharacterParts.generated.h"

class APawn;
class UOWPawnComponent_CharacterParts;
class UObject;
struct FFrame;

enum class ECharacterPartSource : uint8
{
	Natural,

	NaturalSuppressedViaCheat,

	AppliedViaDeveloperSettingsCheat,

	AppliedViaCheatManager
};

//////////////////////////////////////////////////////////////////////

// 컨트롤러 컴포넌트에서 요청한 캐릭터 파츠
USTRUCT()
struct FOWControllerCharacterPartEntry
{
	GENERATED_BODY()

	FOWControllerCharacterPartEntry()
	{}

public:
	// 이 항목이 나타내는 캐릭터 파츠
	UPROPERTY(EditAnywhere, meta=(ShowOnlyInnerProperties))
	FOWCharacterPart Part;

	// 이미 Pawn에 적용되었을 때의 핸들
	FOWCharacterPartHandle Handle;

	// 이 파츠의 출처
	ECharacterPartSource Source = ECharacterPartSource::Natural;
};

//////////////////////////////////////////////////////////////////////

// 소유 컨트롤러가 Pawn을 소유할 때 스폰할 코스메틱 액터를 구성하는 컴포넌트
UCLASS(meta = (BlueprintSpawnableComponent))
class UOWControllerComponent_CharacterParts : public UControllerComponent
{
	GENERATED_BODY()

public:
	UOWControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

	// 이 커스터마이징 컴포넌트를 소유한 액터에 캐릭터 파츠를 추가합니다. 서버 권한에서만 호출해야 합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void AddCharacterPart(const FOWCharacterPart& NewPart);

	// 이 커스터마이징 컴포넌트를 소유한 액터에서 이전에 추가한 캐릭터 파츠를 제거합니다. 서버 권한에서만 호출해야 합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(const FOWCharacterPart& PartToRemove);

	// 추가된 모든 캐릭터 파츠를 제거합니다. 서버 권한에서만 호출해야 합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// PIE에서 관련 개발자 설정을 적용합니다.
	void ApplyDeveloperSettings();

protected:
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	TArray<FOWControllerCharacterPartEntry> CharacterParts;

private:
	UOWPawnComponent_CharacterParts* GetPawnCustomizer() const;

	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);

	void AddCharacterPartInternal(const FOWCharacterPart& NewPart, ECharacterPartSource Source);

	void AddCheatPart(const FOWCharacterPart& NewPart, bool bSuppressNaturalParts);
	void ClearCheatParts();

	void SetSuppressionOnNaturalParts(bool bSuppressed);

	friend class UOWCosmeticCheats;
};

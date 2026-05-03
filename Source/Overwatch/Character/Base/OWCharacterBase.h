// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "OWCharacterBase.generated.h"

class UOWCameraComponent;
class UOWPawnExtensionComponent;

/**
 * 프로젝트의 기본 캐릭터 Pawn이다.
 * 새 게임플레이 동작은 가능하면 PawnComponent로 붙이고, 이 클래스는 Pawn 공통 컴포넌트 조립만 담당한다.
 */
UCLASS(abstract)
class OVERWATCH_API AOWCharacterBase : public AModularCharacter
{
	GENERATED_BODY()

public:
	AOWCharacterBase();

protected:
	virtual void PossessedBy(AController* InNewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* InInputComponent) override;

public:
	/** Lyra식 초기화 상태 체인을 조율하는 Pawn 공통 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OW|Character")
	TObjectPtr<UOWPawnExtensionComponent> PawnExtComponent;

	/** 현재 Pawn을 바라보는 카메라 모드 스택의 진입점 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hak|Character")
	TObjectPtr<UOWCameraComponent> CameraComponent;
};


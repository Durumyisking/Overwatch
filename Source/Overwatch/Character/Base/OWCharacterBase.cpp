// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/Base/OWCharacterBase.h"

#include "Camera/OWCameraComponent.h"
#include "Character/Components/OWPawnExtensionComponent.h"

AOWCharacterBase::AOWCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	PawnExtComponent = CreateDefaultSubobject<UOWPawnExtensionComponent>(TEXT("PawnExtensionComponent"));

	// 기본 캐릭터가 카메라 소유권을 갖고, 하위 영웅 클래스는 카메라 모드와 입력만 조립한다.
	CameraComponent = CreateDefaultSubobject<UOWCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
}

void AOWCharacterBase::SetupPlayerInputComponent(UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InInputComponent);

	// 입력 컴포넌트가 준비된 시점에 PawnExtension의 Lyra식 초기화 상태 체인을 진행한다.
	if (PawnExtComponent)
	{
		PawnExtComponent->SetupPlayerInputComponent();
	}
}

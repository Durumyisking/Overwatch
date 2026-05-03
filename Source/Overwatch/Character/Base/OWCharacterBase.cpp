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

void AOWCharacterBase::PossessedBy(AController* InNewController)
{
	Super::PossessedBy(InNewController);

	if (PawnExtComponent)
	{
		// Controller possession이 바뀌면 PawnExtension의 InitState와 ASC ActorInfo를 다시 확인한다.
		PawnExtComponent->HandleControllerChanged();
	}
}

void AOWCharacterBase::UnPossessed()
{
	Super::UnPossessed();

	if (PawnExtComponent)
	{
		// Controller가 제거된 경우에도 PawnExtension이 ASC Avatar 연결 상태를 정리할 수 있어야 한다.
		PawnExtComponent->HandleControllerChanged();
	}
}

void AOWCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	if (PawnExtComponent)
	{
		// 클라이언트에서 Controller 복제가 도착하면 InitState 조건을 다시 평가한다.
		PawnExtComponent->HandleControllerChanged();
	}
}

void AOWCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (PawnExtComponent)
	{
		// 클라이언트에서 PlayerState 복제가 도착하면 Hero/PawnExtension 초기화 조건을 다시 평가한다.
		PawnExtComponent->HandlePlayerStateReplicated();
	}
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

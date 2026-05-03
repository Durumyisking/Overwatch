// Fill out your copyright notice in the Description page of Project Settings.


#include "ModularCharacter.h"
#include "Components/GameFrameworkComponentManager.h"


void AModularCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularCharacter::BeginPlay()
{
	// AModularCharacter는 준비된 시점에 게임 프레임워크 컴포넌트 매니저에 게임 액터 준비 이벤트를 보낸다. 이 이벤트를 받은 컴포넌트들은 자신이 준비된 것으로 간주하고 초기화 절차를 이어갈 수 있다.
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AModularCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}


#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "OWPawnExtensionComponent.generated.h"

class UOWPawnData;
class UOWAbilitySystemComponent;

/**
 * PawnData, Controller, PlayerState, 입력 컴포넌트 준비 상태를 하나의 InitState 흐름으로 묶는 조율자다.
 * Pawn 자체가 직접 게임 규칙을 해석하지 않도록, 다른 PawnComponent들이 이 상태 체인을 기준으로 초기화된다.
 */
UCLASS()
class OVERWATCH_API UOWPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UOWPawnExtensionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** GameFrameworkComponentManager에 등록되는 이 컴포넌트의 기능 이름 */
	static const FName NAME_ActorFeatureName;

	/** 지정 Actor에 붙어 있는 PawnExtensionComponent를 찾는다. */
	static UOWPawnExtensionComponent* FindPawnExtensionComponent(const AActor* InActor)
	{
		return InActor ? InActor->FindComponentByClass<UOWPawnExtensionComponent>() : nullptr;
	}

	/** 현재 PawnData를 호출자가 기대하는 구체 타입으로 조회한다. */
	template <class T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	void SetPawnData(const UOWPawnData* InPawnData);
	UOWAbilitySystemComponent* GetOWAbilitySystemComponent() const { return AbilitySystemComponent; }
	void InitializeAbilitySystem(UOWAbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UninitializeAbilitySystem();
	void HandleControllerChanged();
	void HandlePlayerStateReplicated();
	void SetupPlayerInputComponent();
	void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate InDelegate);
	void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate InDelegate);

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) const override;
	virtual void CheckDefaultInitialization() override;

protected:
	UFUNCTION()
	void OnRep_PawnData();

	/** 이 Pawn의 클래스, 입력, 카메라 기본값을 정의하는 복제 데이터 */
	UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_PawnData, Category = "OW|Pawn")
	TObjectPtr<const UOWPawnData> PawnData;

	UPROPERTY()
	TObjectPtr<UOWAbilitySystemComponent> AbilitySystemComponent;

	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
};

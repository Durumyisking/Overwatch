// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystemComponent.h"
#include "OWAbilitySet.generated.h"

class UOWAbilitySystemComponent;
class UOWGameplayAbility;

/** AbilitySet이 ASC에 부여할 GameplayAbility와 입력 태그 설정 */
USTRUCT(BlueprintType)
struct FOWAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// 허용된 GameplayAbility
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UOWGameplayAbility> Ability = nullptr;

	// Input처리를 위한 GameplayTag
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;

	// Ability의 금지 조건
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> BlockedTag;
};


/** AbilitySet이 부여한 AbilitySpecHandle을 저장해 나중에 같은 묶음으로 회수한다. */
USTRUCT(BlueprintType)
struct FOWAbilitySet_GrantedHandles
{
	GENERATED_BODY()
	
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void TakeFromAbilitySystem(UOWAbilitySystemComponent* InASC) const;

	// 허용된 GameplayAbilitySpecHandle (int32)
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
};

/**
 * 영웅, 장비, Experience가 ASC에 부여할 능력 묶음을 표현하는 데이터 에셋이다.
 * 부여한 항목은 GrantedHandles로 추적해 제거 시점도 같은 책임 안에서 처리한다.
 */
UCLASS()
class OVERWATCH_API UOWAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	
	/** 이 세트가 ASC에 부여할 Ability 목록 */
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FOWAbilitySet_GameplayAbility> Abilities;

public:
	/** 지정 ASC에 AbilitySet을 부여하고, 필요하면 회수용 핸들을 기록한다. */
	void GiveAbilitySystem(UOWAbilitySystemComponent* InASC, FOWAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
};

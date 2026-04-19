// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AbilitySystemComponent.h"

/*
 * GA의 WrapperClass 
 */

class UOWAbilitySystemComponent;
class UOWGameplayAbility;

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


USTRUCT(BlueprintType)
struct FOWAbilitySet_GrandtedHandles
{
	GENERATED_BODY()
	
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void TakeFromAbilitySystem(UOWAbilitySystemComponent* InASC) const;
};
UCLASS()
class OVERWATCH_API UOWAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<FOWAbilitySet_GameplayAbility> Abilities;

public:
	void GiveAbilitySystem(UOWAbilitySystemComponent* AbilitySystemComponent, FOWAbilitySet_GrandtedHandles* OutGrantedHandles, UObject* SourceObject) const;
};

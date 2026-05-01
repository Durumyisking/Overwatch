#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "OWInputConfig.generated.h"

class UInputAction;

/** InputAction 하나를 게임플레이 입력 태그에 매핑한다. */
USTRUCT(BlueprintType)
struct FOWInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * Pawn 입력 설정을 담는 데이터 에셋이다.
 * NativeInputActions는 직접 바인딩하고, AbilityInputActions는 같은 입력 태그를 가진 Ability로 전달한다.
 */
UCLASS(BlueprintType)
class OVERWATCH_API UOWInputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UOWInputConfig(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InInputTag, bool bLogIfNotFound = true) const;
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InInputTag, bool bLogIfNotFound = true) const;

	/** 소유자가 직접 처리할 입력 액션 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FOWInputAction> NativeInputActions;

	/** 입력 태그가 일치하는 Ability로 자동 전달할 입력 액션 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FOWInputAction> AbilityInputActions;
};

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/OWInputConfig.h"
#include "OWInputComponent.generated.h"

UCLASS()
class OVERWATCH_API UOWInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UOWInputComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	template <class UserClass, typename FuncType>
	void BindNativeAction(const UOWInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent, UserClass* InObject, FuncType InFunc, bool bLogIfNotFound = true);

	template <class UserClass, typename FuncType>
	void BindAbilityActions(const UOWInputConfig* InInputConfig, ETriggerEvent InTriggerEvent, UserClass* InObject, FuncType InFunc);
};

template <class UserClass, typename FuncType>
void UOWInputComponent::BindNativeAction(const UOWInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent, UserClass* InObject, FuncType InFunc, bool bLogIfNotFound)
{
	check(InInputConfig);

	if (const UInputAction* InputAction = InInputConfig->FindNativeInputActionForTag(InInputTag, bLogIfNotFound))
	{
		BindAction(InputAction, InTriggerEvent, InObject, InFunc);
	}
}

template <class UserClass, typename FuncType>
void UOWInputComponent::BindAbilityActions(const UOWInputConfig* InInputConfig, ETriggerEvent InTriggerEvent, UserClass* InObject, FuncType InFunc)
{
	check(InInputConfig);

	for (const FOWInputAction& Action : InInputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			BindAction(Action.InputAction, InTriggerEvent, InObject, InFunc, Action.InputTag);
		}
	}
}

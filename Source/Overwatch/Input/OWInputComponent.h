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

	/** 입력 태그에 대응하는 Native InputAction을 찾아 지정 TriggerEvent에 바인딩한다. */
	template <class UserClass, typename FuncType>
	void BindNativeAction(const UOWInputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent InTriggerEvent, UserClass* InObject, FuncType InFunc, bool bLogIfNotFound = true);

	/** AbilityInputActions에 등록된 모든 액션을 입력 태그와 함께 ASC 전달 함수에 바인딩한다. */
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UOWInputConfig* InInputConfig, UserClass* InObject, PressedFuncType InPressedFunc, ReleasedFuncType InReleasedFunc, TArray<uint32>& OutBindHandles);
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

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UOWInputComponent::BindAbilityActions(const UOWInputConfig* InInputConfig, UserClass* InObject, PressedFuncType InPressedFunc, ReleasedFuncType InReleasedFunc, TArray<uint32>& OutBindHandles)
{
	check(InInputConfig);

	for (const FOWInputAction& Action : InInputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (InPressedFunc)
			{
				// Triggered는 Ability 입력 Pressed로 취급하고, InputTag를 함께 전달한다.
				OutBindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, InObject, InPressedFunc, Action.InputTag).GetHandle());
			}

			if (InReleasedFunc)
			{
				// Completed는 Ability 입력 Released로 취급하고, InputTag를 함께 전달한다.
				OutBindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, InObject, InReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}

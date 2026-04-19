#include "Input/OWInputConfig.h"

#include "OWLog.h"

UOWInputConfig::UOWInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UOWInputConfig::FindNativeInputActionForTag(const FGameplayTag& InInputTag, bool bLogIfNotFound) const
{
	for (const FOWInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && Action.InputTag == InInputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogIfNotFound)
	{
		OW_LOG(LogOWGame, Error, "Can't find NativeInputAction for InputTag [%s] on InputConfig [%s].", *InInputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UOWInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InInputTag, bool bLogIfNotFound) const
{
	for (const FOWInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InInputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogIfNotFound)
	{
		OW_LOG(LogOWGame, Error, "Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s].", *InInputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

#pragma once

#include "OWMappableConfigPair.generated.h"

class UPlayerMappableInputConfig;

USTRUCT()
struct FOWMappableConfigPair
{
	GENERATED_BODY()

public:
	FOWMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};

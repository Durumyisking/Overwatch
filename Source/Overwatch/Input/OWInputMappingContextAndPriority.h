#pragma once

#include "CoreMinimal.h"
#include "OWInputMappingContextAndPriority.generated.h"

class UInputMappingContext;

/** EnhancedInput에 등록할 InputMappingContext와 우선순위, 설정 UI 등록 여부를 함께 보관한다. */
USTRUCT(BlueprintType)
struct FOWInputMappingContextAndPriority
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMapping;

	UPROPERTY(EditAnywhere, Category = "Input")
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, Category = "Input")
	bool bRegisterWithSettings = true;
};

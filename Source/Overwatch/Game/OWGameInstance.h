#pragma once

#include "Engine/GameInstance.h"
#include "OWGameInstance.generated.h"

UCLASS()
class OVERWATCH_API UOWGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;
};

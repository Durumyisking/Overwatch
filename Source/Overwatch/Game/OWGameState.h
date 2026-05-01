#pragma once

#include "GameFramework/GameStateBase.h"
#include "OWGameState.generated.h"

class UOWExperienceManagerComponent;

/** 매치 전역 복제 상태의 소유자이며, Experience 관리 컴포넌트를 통해 런타임 기능을 조립한다. */
UCLASS()
class OVERWATCH_API AOWGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AOWGameState();

	/** 현재 매치 Experience의 로드/활성화 상태를 관리하는 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OW|Experience")
	TObjectPtr<UOWExperienceManagerComponent> ExperienceManagerComponent;
};

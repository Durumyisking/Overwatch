#pragma once

#include "Engine/DataAsset.h"
#include "OWExperienceActionSet.generated.h"

class UGameFeatureAction;

/** Experience 진입 시 함께 실행할 GameFeatureAction 묶음이다. */
UCLASS()
class OVERWATCH_API UOWExperienceActionSet : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOWExperienceActionSet();

	/** 이 액션 세트가 로드/활성화/비활성화 과정에서 실행할 액션 목록 */
	UPROPERTY(EditAnywhere, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

	/** 이 액션 세트가 함께 활성화할 GameFeature 플러그인 목록 */
	UPROPERTY(EditAnywhere, Category = "Gameplay")
	TArray<FString> GameFeaturesToEnable;
};

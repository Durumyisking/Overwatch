#pragma once

#include "Engine/DataAsset.h"
#include "OWExperienceDefinition.generated.h"

class UGameFeatureAction;
class UOWExperienceActionSet;
class UOWPawnData;

/**
 * 한 판 또는 프런트엔드 화면을 조립하기 위한 Experience 정의다.
 * GameMode는 규칙을 직접 하드코딩하지 않고 이 에셋을 선택해 GameFeature, PawnData, ActionSet을 조립한다.
 */
UCLASS()
class OVERWATCH_API UOWExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOWExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 플레이어에게 기본으로 적용할 Pawn 데이터 */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TObjectPtr<UOWPawnData> DefaultPawnData;

	/** 이 Experience가 활성화할 GameFeature 플러그인 목록 */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<FString> GameFeaturesToEnable;

	/** 이 Experience에 추가로 합성할 액션 세트 목록 */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	TArray<TObjectPtr<UOWExperienceActionSet>> ActionSets;

	/** Experience 로드/활성화/비활성화 흐름에서 실행할 액션 목록 */
	UPROPERTY(EditDefaultsOnly, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;
};

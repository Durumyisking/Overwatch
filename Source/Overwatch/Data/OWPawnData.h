#pragma once

#include "Engine/DataAsset.h"
#include "OWPawnData.generated.h"

class UOWInputConfig;
class UOWCameraMode;

/**
 * Pawn을 정의하는 불변 데이터 에셋이다.
 * Experience나 PlayerState는 이 데이터를 통해 Pawn 클래스, 입력, 기본 카메라 모드를 조립한다.
 */
UCLASS()
class OVERWATCH_API UOWPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UOWPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 이 데이터로 생성할 Pawn 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Pawn")
	TSubclassOf<APawn> PawnClass;

	/** 플레이어가 조종할 때 사용할 기본 카메라 모드 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Camera")
	TSubclassOf<UOWCameraMode> DefaultCameraMode;

	/** 플레이어 Pawn의 입력 매핑과 액션 바인딩에 사용할 입력 설정 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OW|Input")
	TObjectPtr<UOWInputConfig> InputConfig;
};

#pragma once

#include "AbilitySystem/OWAbilitySet.h"
#include "Templates/SubclassOf.h"
#include "OWEquipmentDefinition.generated.h"

class AActor;
class UOWEquipmentInstance;

/** 장비가 Pawn에 붙일 표현용 Actor와 부착 위치를 정의한다. */
USTRUCT()
struct FOWEquipmentActorToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Equipment)
	TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
	FTransform AttachTransform;
};

/** 장착 가능한 장비의 데이터 정의다. 런타임 상태는 EquipmentInstance가 소유한다. */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class OVERWATCH_API UOWEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UOWEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** 이 장비가 장착될 때 생성할 런타임 인스턴스 타입 */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TSubclassOf<UOWEquipmentInstance> InstanceType;

	/** 장착 중 ASC에 부여하고 해제 시 회수할 AbilitySet 목록 */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<TObjectPtr<const UOWAbilitySet>> AbilitySetsToGrant;

	/** 장착 중 Pawn에 부착할 표현용 Actor 목록 */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FOWEquipmentActorToSpawn> ActorsToSpawn;
};

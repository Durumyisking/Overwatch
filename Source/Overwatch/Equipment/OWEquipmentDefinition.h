#pragma once

#include "AbilitySystem/OWAbilitySet.h"
#include "Templates/SubclassOf.h"
#include "OWEquipmentDefinition.generated.h"

class AActor;
class UOWEquipmentInstance;

/** 장착 가능한 장비의 정적 정의다. 어떤 런타임 인스턴스를 만들고, 장착 중 무엇을 부여/스폰할지 선언한다. */
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
	/** 장착 가능한 장비의 데이터 정의다. 런타임 상태는 EquipmentInstance가 소유한다. */
	// 인스턴스 객체는 맞음 맞는데 Equipment주체가 아니라 해당 Equipment를 관리하는 방식을 결정하는 클래스라고 생각하면 될듯

	/** 이 장비가 장착될 때 생성할 런타임 상태 객체 타입이다. 장비별 동작 차이는 이 타입의 override로 확장한다. */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TSubclassOf<UOWEquipmentInstance> InstanceType;

	/** 장착 중 ASC에 부여하고 해제 시 회수할 AbilitySet 목록이다. 부여/회수 핸들은 EquipmentList 엔트리가 소유한다. */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<TObjectPtr<const UOWAbilitySet>> AbilitySetsToGrant;

	/** 장착 중 Pawn에 부착할 표현용 Actor 정의 목록이다. 실제 스폰/제거된 Actor의 런타임 상태는 EquipmentInstance가 소유한다. */
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
	TArray<FOWEquipmentActorToSpawn> ActorsToSpawn;
};

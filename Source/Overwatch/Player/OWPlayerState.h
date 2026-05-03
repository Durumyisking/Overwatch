#pragma once

#include "GameFramework/PlayerState.h"
#include "OWPlayerState.generated.h"

class UOWAbilitySystemComponent;
class UOWExperienceDefinition;
class UOWPawnData;

/**
 * 플레이어의 지속 상태를 소유하는 복제 객체다.
 * ASC와 PawnData는 리스폰을 넘어 유지되어야 하므로 Pawn이 아니라 PlayerState에 둔다.
 */
UCLASS()
class OVERWATCH_API AOWPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AOWPlayerState();
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UOWAbilitySystemComponent* GetAbilitySystemComponent() const;
	UOWAbilitySystemComponent* GetOWAbilitySystemComponent() const;

	/** 현재 플레이어에게 적용된 PawnData를 호출자가 기대하는 구체 타입으로 조회한다. */
	template <class T>
	const T* GetPawnData() const
	{
		return Cast<T>(PawnData);
	}

	void OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience);
	void SetPawnData(const UOWPawnData* InPawnData);

protected:
	UFUNCTION()
	void OnRep_PawnData();

	/** 플레이어 캐릭터가 사용하는 AbilitySystemComponent 서브오브젝트 */
	UPROPERTY(VisibleAnywhere, Category = "OW|PlayerState")
	TObjectPtr<UOWAbilitySystemComponent> AbilitySystemComponent;

	/** Experience 또는 선택 흐름에서 결정된 Pawn 구성 데이터 */
	UPROPERTY(ReplicatedUsing = OnRep_PawnData)
	TObjectPtr<const UOWPawnData> PawnData;
};

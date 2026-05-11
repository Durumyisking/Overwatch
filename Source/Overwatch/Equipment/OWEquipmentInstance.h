#pragma once

#include "Engine/World.h"
#include "OWEquipmentInstance.generated.h"

class AActor;
class APawn;
struct FOWEquipmentActorToSpawn;

/** Pawn에 적용된 장비의 런타임 상태다. 정의 데이터와 달리 장착 생명주기와 스폰 Actor를 소유한다. */
UCLASS(BlueprintType, Blueprintable)
class OVERWATCH_API UOWEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UOWEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final;

	UFUNCTION(BlueprintPure, Category = Equipment)
	UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category = Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category = Equipment)
	TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	virtual void SpawnEquipmentActors(const TArray<FOWEquipmentActorToSpawn>& ActorsToSpawn);
	virtual void DestroyEquipmentActors();

	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:
	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnUnequipped"))
	void K2_OnUnequipped();

private:
	UFUNCTION()
	void OnRep_Instigator();

	// 어떤 InventoryItemInstance에 의해 활성화되었는지 
	UPROPERTY(ReplicatedUsing = OnRep_Instigator)
	TObjectPtr<UObject> Instigator;

	// UOWEquipmentDefinition에 의해 Spawn된 Actor들. 장비가 해제될 때 제거한다.
	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;
};

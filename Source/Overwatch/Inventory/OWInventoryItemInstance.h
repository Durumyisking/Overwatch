#pragma once

#include "Core/Types/OWGameplayTagStack.h"
#include "Templates/SubclassOf.h"
#include "OWInventoryItemInstance.generated.h"

class UOWInventoryItemDefinition;
class UOWInventoryItemFragment;

/** Inventory에 들어간 실제 아이템 인스턴스다. Definition 참조와 런타임 StatTag를 복제한다. */
UCLASS(BlueprintType)
class OVERWATCH_API UOWInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UOWInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual bool IsSupportedForNetworking() const override { return true; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void AddStatTagStack(FGameplayTag InTag, int32 InStackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
	void RemoveStatTagStack(FGameplayTag InTag, int32 InStackCount);

	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetStatTagStackCount(FGameplayTag InTag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool HasStatTag(FGameplayTag InTag) const;

	TSubclassOf<UOWInventoryItemDefinition> GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Inventory, meta = (DeterminesOutputType = FragmentClass))
	const UOWInventoryItemFragment* FindFragmentByClass(TSubclassOf<UOWInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return Cast<ResultClass>(FindFragmentByClass(ResultClass::StaticClass()));
	}

private:
	friend struct FOWInventoryList;

	virtual void RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags) override;

	void SetItemDef(TSubclassOf<UOWInventoryItemDefinition> InItemDef);

	UPROPERTY(Replicated)
	FOWGameplayTagStackContainer StatTags;

	UPROPERTY(Replicated)
	TSubclassOf<UOWInventoryItemDefinition> ItemDef;
};

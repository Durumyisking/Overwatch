#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "OWInventoryItemDefinition.generated.h"

class UOWInventoryItemInstance;

/** 아이템 정의에 붙는 조각 데이터다. 장비 가능 여부, 초기 스탯 같은 확장축은 Fragment 타입으로 추가한다. */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OVERWATCH_API UOWInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UOWInventoryItemInstance* Instance) const {}
};

/** Inventory 아이템의 데이터 정의다. 런타임 개수와 상태는 ItemInstance가 소유한다. */
UCLASS(Blueprintable, Const, Abstract)
class OVERWATCH_API UOWInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UOWInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	const UOWInventoryItemFragment* FindFragmentByClass(TSubclassOf<UOWInventoryItemFragment> FragmentClass) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
	TArray<TObjectPtr<UOWInventoryItemFragment>> Fragments;
};

/** Blueprint에서 ItemDefinition의 Fragment를 타입 안전하게 찾기 위한 얇은 조회 함수다. */
UCLASS()
class OVERWATCH_API UOWInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Inventory, meta = (DeterminesOutputType = FragmentClass))
	static const UOWInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UOWInventoryItemDefinition> ItemDefinition, TSubclassOf<UOWInventoryItemFragment> FragmentClass);
};

#pragma once

#include "Cosmetics/OWCosmeticAnimationTypes.h"
#include "Equipment/OWEquipmentInstance.h"
#include "GameFramework/InputDevicePropertyHandle.h"
#include "OWWeaponInstance.generated.h"

class UAnimInstance;
class UInputDeviceProperty;

/** EquipmentInstance를 무기 용도로 확장한 런타임 인스턴스다. */
UCLASS()
class OVERWATCH_API UOWWeaponInstance : public UOWEquipmentInstance
{
	GENERATED_BODY()

public:
	UOWWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	/** 발사 시점을 기록한다. 연사/조준 UI는 이 시간을 기준으로 최근 상호작용 여부를 판단한다. */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void UpdateFiringTime();

	/** 장착 또는 발사 이후 지난 시간을 반환한다. */
	UFUNCTION(BlueprintPure, Category = Weapon)
	float GetTimeSinceLastInteractedWith() const;

	/** 현재 코스메틱 태그에 가장 잘 맞는 무기 애니메이션 레이어를 선택한다. */
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Animation)
	TSubclassOf<UAnimInstance> PickBestAnimLayer(bool bEquipped, const FGameplayTagContainer& CosmeticTags) const;

protected:
	/** 장착 중 사용할 애니메이션 레이어 선택 규칙 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FOWAnimLayerSelectionSet EquippedAnimSet;

	/** 비장착 상태에서 사용할 애니메이션 레이어 선택 규칙 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	FOWAnimLayerSelectionSet UnequippedAnimSet;

	/** 장착 중 적용할 입력 장치 피드백 속성이다. 해제 시 핸들로 반드시 회수한다. */
	UPROPERTY(EditDefaultsOnly, Instanced, BlueprintReadOnly, Category = "Input Devices")
	TArray<TObjectPtr<UInputDeviceProperty>> ApplicableDeviceProperties;

	FPlatformUserId GetOwningUserId() const;
	void ApplyDeviceProperties();
	void RemoveDeviceProperties();

private:
	UPROPERTY(Transient)
	TSet<FInputDevicePropertyHandle> DevicePropertyHandles;

	double TimeLastEquipped = 0.0;
	double TimeLastFired = 0.0;
};

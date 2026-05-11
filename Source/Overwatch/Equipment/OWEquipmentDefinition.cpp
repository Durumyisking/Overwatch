#include "Equipment/OWEquipmentDefinition.h"

#include "Equipment/OWEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWEquipmentDefinition)

UOWEquipmentDefinition::UOWEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 혹시나 없을수도 있으니까..
	InstanceType = UOWEquipmentInstance::StaticClass();
}

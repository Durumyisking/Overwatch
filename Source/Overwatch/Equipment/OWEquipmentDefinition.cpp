#include "Equipment/OWEquipmentDefinition.h"

#include "Equipment/OWEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWEquipmentDefinition)

UOWEquipmentDefinition::UOWEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UOWEquipmentInstance::StaticClass();
}

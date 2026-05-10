#include "Equipment/OWEquipmentInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Equipment/OWEquipmentDefinition.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Iris/ReplicationSystem/ReplicationFragmentUtil.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWEquipmentInstance)

class FLifetimeProperty;

UOWEquipmentInstance::UOWEquipmentInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UWorld* UOWEquipmentInstance::GetWorld() const
{
	if (APawn* OwningPawn = GetPawn())
	{
		return OwningPawn->GetWorld();
	}

	return nullptr;
}

void UOWEquipmentInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

void UOWEquipmentInstance::RegisterReplicationFragments(UE::Net::FFragmentRegistrationContext& Context, UE::Net::EFragmentRegistrationFlags RegistrationFlags)
{
	UE::Net::FReplicationFragmentUtil::CreateAndRegisterFragmentsForObject(this, Context, RegistrationFlags);
}

APawn* UOWEquipmentInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn* UOWEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	if (UClass* ActualPawnType = PawnType)
	{
		if (GetOuter() && GetOuter()->IsA(ActualPawnType))
		{
			return Cast<APawn>(GetOuter());
		}
	}

	return nullptr;
}

void UOWEquipmentInstance::SpawnEquipmentActors(const TArray<FOWEquipmentActorToSpawn>& ActorsToSpawn)
{
	APawn* OwningPawn = GetPawn();
	UWorld* World = GetWorld();
	if (!OwningPawn || !World)
	{
		return;
	}

	// Lyra와 같이 Character는 Mesh에, 그 외 Pawn은 RootComponent에 장비 Actor를 붙인다.
	USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
	if (ACharacter* Character = Cast<ACharacter>(OwningPawn))
	{
		AttachTarget = Character->GetMesh();
	}

	if (!AttachTarget)
	{
		return;
	}

	for (const FOWEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
	{
		if (!SpawnInfo.ActorToSpawn)
		{
			continue;
		}

		AActor* NewActor = World->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
		if (!NewActor)
		{
			continue;
		}

		NewActor->FinishSpawning(FTransform::Identity, true);
		NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
		NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);
		SpawnedActors.Add(NewActor);
	}
}

void UOWEquipmentInstance::DestroyEquipmentActors()
{
	for (AActor* Actor : SpawnedActors)
	{
		if (Actor)
		{
			Actor->Destroy();
		}
	}

	SpawnedActors.Reset();
}

void UOWEquipmentInstance::OnEquipped()
{
	K2_OnEquipped();
}

void UOWEquipmentInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void UOWEquipmentInstance::OnRep_Instigator()
{
}

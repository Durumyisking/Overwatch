#include "Character/Components/OWPawnExtensionComponent.h"

#include "Components/GameFrameworkComponentManager.h"
#include "Core/Types/OWGameplayTags.h"
#include "Data/OWPawnData.h"
#include "OWLog.h"

const FName UOWPawnExtensionComponent::NAME_ActorFeatureName(TEXT("PawnExtension"));

UOWPawnExtensionComponent::UOWPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UOWPawnExtensionComponent::SetPawnData(const UOWPawnData* InPawnData)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData || !InPawnData)
	{
		return;
	}

	PawnData = InPawnData;
}

void UOWPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	if (!GetPawn<APawn>())
	{
		OW_ERROR(LogOWGame, "OWPawnExtensionComponent requires a Pawn owner.");
		return;
	}

	RegisterInitStateFeature();
}

void UOWPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
	ensure(TryToChangeInitState(FOWGameplayTags::Get().InitState_Spawned));
	CheckDefaultInitialization();
}

void UOWPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

void UOWPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName == NAME_ActorFeatureName)
	{
		return;
	}

	if (Params.FeatureState == FOWGameplayTags::Get().InitState_DataAvailable)
	{
		CheckDefaultInitialization();
	}
}

bool UOWPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* InManager, FGameplayTag InCurrentState, FGameplayTag InDesiredState) const
{
	check(InManager);

	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	APawn* Pawn = GetPawn<APawn>();

	if (!InCurrentState.IsValid() && InDesiredState == GameplayTags.InitState_Spawned)
	{
		return Pawn != nullptr;
	}

	if (InCurrentState == GameplayTags.InitState_Spawned && InDesiredState == GameplayTags.InitState_DataAvailable)
	{
		if (!PawnData)
		{
			return false;
		}

		if (Pawn && Pawn->IsLocallyControlled() && !GetController<AController>())
		{
			return false;
		}

		return true;
	}

	if (InCurrentState == GameplayTags.InitState_DataAvailable && InDesiredState == GameplayTags.InitState_DataInitialized)
	{
		return InManager->HaveAllFeaturesReachedInitState(Pawn, GameplayTags.InitState_DataAvailable);
	}

	if (InCurrentState == GameplayTags.InitState_DataInitialized && InDesiredState == GameplayTags.InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UOWPawnExtensionComponent::CheckDefaultInitialization()
{
	CheckDefaultInitializationForImplementers();

	const FOWGameplayTags& GameplayTags = FOWGameplayTags::Get();
	static const TArray<FGameplayTag> StateChain = { GameplayTags.InitState_Spawned, GameplayTags.InitState_DataAvailable, GameplayTags.InitState_DataInitialized, GameplayTags.InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

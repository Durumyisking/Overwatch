#include "OWGameMode.h"

#include "Character/Components/OWPawnExtensionComponent.h"
#include "Data/OWPawnData.h"
#include "Game/OWExperienceDefinition.h"
#include "Game/OWExperienceManagerComponent.h"
#include "Game/OWGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OWPlayerController.h"
#include "Player/OWPlayerState.h"

AOWGameMode::AOWGameMode()
{
	GameStateClass = AOWGameState::StaticClass();
	PlayerControllerClass = AOWPlayerController::StaticClass();
	PlayerStateClass = AOWPlayerState::StaticClass();
}

void AOWGameMode::InitGame(const FString& InMapName, const FString& InOptions, FString& OutErrorMessage)
{
	Super::InitGame(InMapName, InOptions, OutErrorMessage);

	// GameState components finish their startup after InitGame, so defer Experience assignment one tick.
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
}

void AOWGameMode::InitGameState()
{
	Super::InitGameState();

	// GameMode only orchestrates the server flow. The GameState component owns Experience loading.
	UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
}

UClass* AOWGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (const UOWPawnData* PawnData = GetPawnDataForController(InController))
	{
		if (PawnData->PawnClass)
		{
			return PawnData->PawnClass;
		}
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

void AOWGameMode::HandleStartingNewPlayer_Implementation(APlayerController* InNewPlayer)
{
	// Players start only after Experience data is loaded, because PawnData decides the Pawn class.
	if (IsExperienceLoaded())
	{
		Super::HandleStartingNewPlayer_Implementation(InNewPlayer);
	}
}

APawn* AOWGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* InNewPlayer, const FTransform& InSpawnTransform)
{
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.ObjectFlags |= RF_Transient;
	SpawnInfo.bDeferConstruction = true;

	if (UClass* PawnClass = GetDefaultPawnClassForController(InNewPlayer))
	{
		if (APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(PawnClass, InSpawnTransform, SpawnInfo))
		{
			if (UOWPawnExtensionComponent* PawnExtensionComponent = UOWPawnExtensionComponent::FindPawnExtensionComponent(SpawnedPawn))
			{
				if (const UOWPawnData* PawnData = GetPawnDataForController(InNewPlayer))
				{
					PawnExtensionComponent->SetPawnData(PawnData);
				}
			}

			SpawnedPawn->FinishSpawning(InSpawnTransform);
			return SpawnedPawn;
		}
	}

	return nullptr;
}

void AOWGameMode::HandleMatchAssignmentIfNotExpectingOne()
{
	FPrimaryAssetId ExperienceId;

	// URL option has the highest priority so playlists or launch commands can select the Experience.
	if (!ExperienceId.IsValid() && UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UOWExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}

	// Fall back to the project default Experience when no external assignment exists.
	if (!ExperienceId.IsValid())
	{
		ExperienceId = FPrimaryAssetId(FPrimaryAssetType(UOWExperienceDefinition::StaticClass()->GetFName()), FName(TEXT("BP_LyraDefaultExperience")));
	}

	OnMatchAssignmentGiven(ExperienceId);
}

void AOWGameMode::OnMatchAssignmentGiven(FPrimaryAssetId InExperienceId)
{
	check(InExperienceId.IsValid());

	UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	ExperienceManagerComponent->ServerSetCurrentExperience(InExperienceId);
}

bool AOWGameMode::IsExperienceLoaded() const
{
	check(GameState);
	const UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	return ExperienceManagerComponent->IsExperienceLoaded();
}

void AOWGameMode::OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience)
{
	// Late-start players are held until Experience load completes, then spawned through the normal restart path.
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Cast<APlayerController>(*Iterator);
		if (PlayerController && PlayerController->GetPawn() == nullptr && PlayerCanRestart(PlayerController))
		{
			RestartPlayer(PlayerController);
		}
	}
}

const UOWPawnData* AOWGameMode::GetPawnDataForController(const AController* InController) const
{
	// PlayerState can override PawnData later for hero selection or playlist-specific assignment.
	if (InController)
	{
		if (const AOWPlayerState* PlayerState = InController->GetPlayerState<AOWPlayerState>())
		{
			if (const UOWPawnData* PawnData = PlayerState->GetPawnData<UOWPawnData>())
			{
				return PawnData;
			}
		}
	}

	// Otherwise, use the Experience default. GameMode orchestrates; Experience owns the default data.
	check(GameState);
	const UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	check(ExperienceManagerComponent);

	if (ExperienceManagerComponent->IsExperienceLoaded())
	{
		const UOWExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
		if (Experience->DefaultPawnData)
		{
			return Experience->DefaultPawnData;
		}
	}

	return nullptr;
}

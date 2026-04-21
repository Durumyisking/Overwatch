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

	RequestedExperienceId = FPrimaryAssetId();
	if (UGameplayStatics::HasOption(OptionsString, TEXT("Experience")))
	{
		const FString ExperienceFromOptions = UGameplayStatics::ParseOption(OptionsString, TEXT("Experience"));
		RequestedExperienceId = FPrimaryAssetId(FPrimaryAssetType(UOWExperienceDefinition::StaticClass()->GetFName()), FName(*ExperienceFromOptions));
	}
	else if (DefaultExperienceId.IsValid())
	{
		RequestedExperienceId = DefaultExperienceId;
	}

	if (RequestedExperienceId.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::HandleMatchAssignmentIfNotExpectingOne);
	}
}

void AOWGameMode::InitGameState()
{
	Super::InitGameState();

	if (!GameState)
	{
		return;
	}

	UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	if (ExperienceManagerComponent)
	{
		ExperienceManagerComponent->CallOrRegister_OnExperienceLoaded(FOnOWExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
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
	if (!RequestedExperienceId.IsValid() || IsExperienceLoaded())
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
	if (RequestedExperienceId.IsValid())
	{
		OnMatchAssignmentGiven(RequestedExperienceId);
	}
}

void AOWGameMode::OnMatchAssignmentGiven(FPrimaryAssetId InExperienceId)
{
	if (!GameState || !InExperienceId.IsValid())
	{
		return;
	}

	UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	if (ExperienceManagerComponent)
	{
		ExperienceManagerComponent->ServerSetCurrentExperience(InExperienceId);
	}
}

bool AOWGameMode::IsExperienceLoaded() const
{
	if (!GameState)
	{
		return false;
	}

	const UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	return ExperienceManagerComponent && ExperienceManagerComponent->IsExperienceLoaded();
}

void AOWGameMode::OnExperienceLoaded(const UOWExperienceDefinition* InCurrentExperience)
{
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

	if (!GameState)
	{
		return nullptr;
	}

	const UOWExperienceManagerComponent* ExperienceManagerComponent = GameState->FindComponentByClass<UOWExperienceManagerComponent>();
	if (!ExperienceManagerComponent || !ExperienceManagerComponent->IsExperienceLoaded())
	{
		return nullptr;
	}

	const UOWExperienceDefinition* Experience = ExperienceManagerComponent->GetCurrentExperienceChecked();
	return Experience ? Experience->DefaultPawnData : nullptr;
}

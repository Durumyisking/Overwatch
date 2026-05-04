// Copyright Epic Games, Inc. All Rights Reserved.

#include "Cosmetics/OWControllerComponent_CharacterParts.h"
#include "Cosmetics/OWCharacterPartTypes.h"
#include "Cosmetics/OWPawnComponent_CharacterParts.h"
#include "GameFramework/CheatManagerDefines.h"
//#include "OWCosmeticDeveloperSettings.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OWControllerComponent_CharacterParts)

//////////////////////////////////////////////////////////////////////

UOWControllerComponent_CharacterParts::UOWControllerComponent_CharacterParts(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOWControllerComponent_CharacterParts::BeginPlay()
{
	Super::BeginPlay();

	// Pawn 소유 변경 이벤트를 수신합니다.
	if (HasAuthority())
	{
		if (AController* OwningController = GetController<AController>())
		{
			OwningController->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);

			if (APawn* ControlledPawn = GetPawn<APawn>())
			{
				OnPossessedPawnChanged(nullptr, ControlledPawn);
			}
		}

		ApplyDeveloperSettings();
	}
}

void UOWControllerComponent_CharacterParts::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RemoveAllCharacterParts();
	Super::EndPlay(EndPlayReason);
}

UOWPawnComponent_CharacterParts* UOWControllerComponent_CharacterParts::GetPawnCustomizer() const
{
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		return ControlledPawn->FindComponentByClass<UOWPawnComponent_CharacterParts>();
	}
	return nullptr;
}

void UOWControllerComponent_CharacterParts::AddCharacterPart(const FOWCharacterPart& NewPart)
{
	AddCharacterPartInternal(NewPart, ECharacterPartSource::Natural);
}

void UOWControllerComponent_CharacterParts::AddCharacterPartInternal(const FOWCharacterPart& NewPart, ECharacterPartSource Source)
{
	FOWControllerCharacterPartEntry& NewEntry = CharacterParts.AddDefaulted_GetRef();
	NewEntry.Part = NewPart;
	NewEntry.Source = Source;

	if (UOWPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		if (NewEntry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
		{
			NewEntry.Handle = PawnCustomizer->AddCharacterPart(NewPart);
		}
	}

}

void UOWControllerComponent_CharacterParts::RemoveCharacterPart(const FOWCharacterPart& PartToRemove)
{
	for (auto EntryIt = CharacterParts.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (FOWCharacterPart::AreEquivalentParts(EntryIt->Part, PartToRemove))
		{
			if (UOWPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
			{
				PawnCustomizer->RemoveCharacterPart(EntryIt->Handle);
			}

			EntryIt.RemoveCurrent();
			break;
		}
	}
}

void UOWControllerComponent_CharacterParts::RemoveAllCharacterParts()
{
	if (UOWPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer())
	{
		for (FOWControllerCharacterPartEntry& Entry : CharacterParts)
		{
			PawnCustomizer->RemoveCharacterPart(Entry.Handle);
		}
	}

	CharacterParts.Reset();
}

void UOWControllerComponent_CharacterParts::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	// 이전 Pawn에서 제거합니다.
	if (UOWPawnComponent_CharacterParts* OldCustomizer = OldPawn ? OldPawn->FindComponentByClass<UOWPawnComponent_CharacterParts>() : nullptr)
	{
		for (FOWControllerCharacterPartEntry& Entry : CharacterParts)
		{
			OldCustomizer->RemoveCharacterPart(Entry.Handle);
			Entry.Handle.Reset();
		}
	}

	// 새 Pawn에 적용합니다.
	if (UOWPawnComponent_CharacterParts* NewCustomizer = NewPawn ? NewPawn->FindComponentByClass<UOWPawnComponent_CharacterParts>() : nullptr)
	{
		for (FOWControllerCharacterPartEntry& Entry : CharacterParts)
		{
			// 이미 있으면 다시 추가하지 않습니다. OldPawn이 null인 상태로도 호출될 수 있습니다.
			if (!Entry.Handle.IsValid() && Entry.Source != ECharacterPartSource::NaturalSuppressedViaCheat)
			{
				Entry.Handle = NewCustomizer->AddCharacterPart(Entry.Part);
			}
		}
	}
}

void UOWControllerComponent_CharacterParts::ApplyDeveloperSettings()
{
//#if UE_WITH_CHEAT_MANAGER
//	const UOWCosmeticDeveloperSettings* Settings = GetDefault<UOWCosmeticDeveloperSettings>();
//
//	// 필요하면 자연 파츠를 억제하거나 억제 해제합니다.
//	const bool bSuppressNaturalParts = (Settings->CheatMode == ECosmeticCheatMode::ReplaceParts) && (Settings->CheatCosmeticCharacterParts.Num() > 0);
//	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
//
//	// 개발자 설정으로 추가된 항목을 제거한 뒤 다시 추가합니다.
//	UOWPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();
//	for (auto It = CharacterParts.CreateIterator(); It; ++It)
//	{
//		if (It->Source == ECharacterPartSource::AppliedViaDeveloperSettingsCheat)
//		{
//			if (PawnCustomizer != nullptr)
//			{
//				PawnCustomizer->RemoveCharacterPart(It->Handle);
//			}
//			It.RemoveCurrent();
//		}
//	}
//
//	// 새 파츠를 추가합니다.
//	for (const FOWCharacterPart& PartDesc : Settings->CheatCosmeticCharacterParts)
//	{
//		AddCharacterPartInternal(PartDesc, ECharacterPartSource::AppliedViaDeveloperSettingsCheat);
//	}
//#endif
}


void UOWControllerComponent_CharacterParts::AddCheatPart(const FOWCharacterPart& NewPart, bool bSuppressNaturalParts)
{
#if UE_WITH_CHEAT_MANAGER
	SetSuppressionOnNaturalParts(bSuppressNaturalParts);
	AddCharacterPartInternal(NewPart, ECharacterPartSource::AppliedViaCheatManager);
#endif
}

void UOWControllerComponent_CharacterParts::ClearCheatParts()
{
#if UE_WITH_CHEAT_MANAGER
	UOWPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	// CheatManager 치트로 추가된 항목을 제거합니다.
	for (auto It = CharacterParts.CreateIterator(); It; ++It)
	{
		if (It->Source == ECharacterPartSource::AppliedViaCheatManager)
		{
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(It->Handle);
			}
			It.RemoveCurrent();
		}
	}

	ApplyDeveloperSettings();
#endif
}

void UOWControllerComponent_CharacterParts::SetSuppressionOnNaturalParts(bool bSuppressed)
{
#if UE_WITH_CHEAT_MANAGER
	UOWPawnComponent_CharacterParts* PawnCustomizer = GetPawnCustomizer();

	for (FOWControllerCharacterPartEntry& Entry : CharacterParts)
	{
		if ((Entry.Source == ECharacterPartSource::Natural) && bSuppressed)
		{
			// 억제합니다.
			if (PawnCustomizer != nullptr)
			{
				PawnCustomizer->RemoveCharacterPart(Entry.Handle);
				Entry.Handle.Reset();
			}
			Entry.Source = ECharacterPartSource::NaturalSuppressedViaCheat;
		}
		else if ((Entry.Source == ECharacterPartSource::NaturalSuppressedViaCheat) && !bSuppressed)
		{
			// 억제 해제합니다.
			if (PawnCustomizer != nullptr)
			{
				Entry.Handle = PawnCustomizer->AddCharacterPart(Entry.Part);
			}
			Entry.Source = ECharacterPartSource::Natural;
		}
	}
#endif
}


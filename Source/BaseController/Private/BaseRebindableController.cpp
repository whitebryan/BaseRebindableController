// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseRebindableController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputLibrary.h" 
#include "Engine/LocalPlayer.h"
#include "PlayerMappableKeySettings.h"
#include "InputMappingContext.h"
#include "KeybindsSaveFile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" 

void ABaseRebindableController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PossessedCharacter = Cast<ABaseCharacter>(NewPawn);
	SetupInputs();
}

void ABaseRebindableController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->ClearActionEventBindings();
		EnhancedInputComponent->ClearActionValueBindings();
		EnhancedInputComponent->ClearDebugKeyBindings();

		BindActions(PlayerInputMappingContext);
	}
}

void ABaseRebindableController::BindActions(UInputMappingContext* Context)
{
	if (Context)
	{
		const TArray<FEnhancedActionKeyMapping>& Mappings = Context->GetMappings();
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
		if (EnhancedInputComponent)
		{
			TSet<const UInputAction*> UniqueActions;
			for (const FEnhancedActionKeyMapping& Keymapping : Mappings)
			{
				UniqueActions.Add(Keymapping.Action);
			}
			for (const UInputAction* UniqueAction : UniqueActions)
			{
				EnhancedInputComponent->BindAction(UniqueAction, ETriggerEvent::Triggered, Cast<UObject>(this), UniqueAction->GetFName());
			}
		}
	}
}

void ABaseRebindableController::SetupInputs()
{
	if (PossessedCharacter)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			FModifyContextOptions Options;
			Options.bForceImmediately = 1;
			Subsystem->AddMappingContext(PlayerInputMappingContext, 1, Options);
		}
	}

	loadKeybinds();
}

//key rebinding
bool ABaseRebindableController::rebindKey(const FName mappingToChange, FInputChord newChord, bool ignoreDuplicateKey)
{
	//Get all actions
	TArray<FEnhancedActionKeyMapping> playerMappableKeys = PlayerInputMappingContext->GetMappings();
	
	//The action found that we want to change
	FEnhancedActionKeyMapping* actionToChange = nullptr;

	//Check to make sure the key isnt alreayd in use and find the index of the action we need
	for (int i = 0; i < playerMappableKeys.Num(); ++i)
	{
		UPlayerMappableKeySettings* playerKeySettings = playerMappableKeys[i].GetPlayerMappableKeySettings();
		if (!ignoreDuplicateKey && playerMappableKeys[i].Key == newChord.Key)
		{
			return false;
		}
		else if (playerKeySettings != nullptr && playerKeySettings->Name == mappingToChange)
		{
			actionToChange = &PlayerInputMappingContext->GetMapping(i);
		}
	}

	
	//Actually change the mapping then request a rebuild and save
	if (actionToChange != nullptr)
	{
		actionToChange->Key = newChord.Key;
		UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(PlayerInputMappingContext);

		saveKeybinds();

		return true;
	}

	return false;
}

void ABaseRebindableController::saveKeybinds()
{
	//Create a save object
	UKeybindsSaveFile* playerSaveGame = Cast<UKeybindsSaveFile>(UGameplayStatics::CreateSaveGameObject(UKeybindsSaveFile::StaticClass()));
	if (playerSaveGame)
	{
		TArray<FEnhancedActionKeyMapping> playerMappableKeys = PlayerInputMappingContext->GetMappings();

		//Add all mappable actions and their keys to a map then save it
		for (FEnhancedActionKeyMapping curMapping : playerMappableKeys)
		{
			//If using UE 5.X < 5.2 change to curMapping.bIsPlayerMappable
			if (curMapping.IsPlayerMappable())
			{
				playerSaveGame->inputMap.Add(curMapping.GetPlayerMappableKeySettings()->Name, curMapping.Key);
			}
		}
	}

	UGameplayStatics::SaveGameToSlot(playerSaveGame, keybindSaveSlotName, 0);
}

void ABaseRebindableController::loadKeybinds()
{
	UKeybindsSaveFile* playerSaveGame = Cast<UKeybindsSaveFile>(UGameplayStatics::LoadGameFromSlot(keybindSaveSlotName, 0));
	if (playerSaveGame)
	{
		TArray<FName> keys;
		playerSaveGame->inputMap.GetKeys(keys);

		//Loop through all mappable keys and bind them to what they should be
		for (FName actionName : keys)
		{
			rebindKey(actionName, playerSaveGame->inputMap[actionName], true);
		}
	}
}

void ABaseRebindableController::resetKeybindsToDefault()
{
	TArray<FEnhancedActionKeyMapping> playerMappedKeys = PlayerInputMappingContext->GetMappings();
	TArray<FEnhancedActionKeyMapping> defaultMappedKeys = DefaultInputMappingContext->GetMappings();

	for (int i = 0; i < playerMappedKeys.Num(); ++i)
	{
		UPlayerMappableKeySettings* playerKeySettings = playerMappedKeys[i].GetPlayerMappableKeySettings();
		for (int j = 0; j < defaultMappedKeys.Num(); ++j)
		{
			UPlayerMappableKeySettings* defaultKeySettings = defaultMappedKeys[j].GetPlayerMappableKeySettings();
			if (playerKeySettings != nullptr && defaultKeySettings != nullptr && playerKeySettings->Name == defaultKeySettings->Name)
			{
				rebindKey(playerKeySettings->Name, defaultMappedKeys[j].Key, true);
				break;
			}
		}
	}
}


//Action functions

void ABaseRebindableController::MoveAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->MoveAction(Value.Get<FVector2D>());
	}
}

void ABaseRebindableController::CameraAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->CameraAction(Value.Get<FVector2D>());
	}
}

void ABaseRebindableController::JumpAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->JumpAction(Value.Get<bool>());
	}
}

void ABaseRebindableController::MenuAction(const FInputActionValue& Value)
{
	if (PossessedCharacter)
	{
		PossessedCharacter->MenuAction();
	}
}
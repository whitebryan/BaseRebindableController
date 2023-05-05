// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseRebindableController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputLibrary.h" 
#include "Engine/LocalPlayer.h"
#include "Engine/Public/SubSystems/LocalPlayerSubsystem.h"
#include "InputMappingContext.h"
#include "KeybindsSaveFile.h"
#include "Kismet/GameplayStatics.h"

void ABaseRebindableController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	PossessedCharacter = Cast<CHARACTER_CLASS>(NewPawn);
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
bool ABaseRebindableController::rebindKey(const FName mappingToChange, FKey newKey, bool ignoreDuplicateKey)
{
	//Get all actions
	TArray<FEnhancedActionKeyMapping> playerMappableKeys = PlayerInputMappingContext->GetMappings();
	
	//The action found that we want to change
	FEnhancedActionKeyMapping* actionToChange = nullptr;

	//Check to make sure the key isnt alreayd in use and find the index of the action we need
	for (int i = 0; i < playerMappableKeys.Num(); ++i)
	{
		if (!ignoreDuplicateKey && playerMappableKeys[i].Key == newKey)
		{
			return false;
		}
		else if (playerMappableKeys[i].PlayerMappableOptions.Name == mappingToChange)
		{
			actionToChange = &PlayerInputMappingContext->GetMapping(i);
		}
	}

	
	//Actually change the mapping then request a rebuild and save
	if (actionToChange->PlayerMappableOptions.Name == mappingToChange)
	{
		actionToChange->Key = newKey;
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
			if (curMapping.IsPlayerMappable())
			{
				playerSaveGame->inputMap.Add(curMapping.PlayerMappableOptions.Name, curMapping.Key);
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
		for (int j = 0; j < defaultMappedKeys.Num(); ++j)
		{
			if (playerMappedKeys[i].PlayerMappableOptions.Name == defaultMappedKeys[j].PlayerMappableOptions.Name)
			{
				rebindKey(playerMappedKeys[i].PlayerMappableOptions.Name, defaultMappedKeys[j].Key, true);
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
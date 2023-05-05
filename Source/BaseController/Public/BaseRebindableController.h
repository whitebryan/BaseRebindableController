// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "PlayerMappableInputConfig.h"
#include "BaseRebindableController.generated.h"

#define CHARACTER_CLASS ABaseCharacter

class UInputMappingContext;
class CHARACTER_CLASS;

UCLASS(Blueprintable, BlueprintType)
class BASECONTROLLER_API ABaseRebindableController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void OnPossess(APawn* NewPawn) override;

	virtual void SetupInputComponent() override;

	virtual void BindActions(UInputMappingContext* Context);

	UFUNCTION(BlueprintCallable)
	bool rebindKey(const FName mappingToChange, FKey newKey, bool ignoreDuplicateKey = false);
	UFUNCTION(BlueprintCallable)
	void resetKeybindsToDefault();

protected:

	void SetupInputs();

	UFUNCTION()
	void MoveAction(const FInputActionValue& Value);

	UFUNCTION()
	void CameraAction(const FInputActionValue& Value);

	UFUNCTION()
	void JumpAction(const FInputActionValue& Value);

	void saveKeybinds();
	void loadKeybinds();

public:
	UPROPERTY(EditDefaultsOnly)
	FString keybindSaveSlotName = "Keybinds";

	TObjectPtr<CHARACTER_CLASS> PossessedCharacter = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> PlayerInputMappingContext = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UInputMappingContext> DefaultInputMappingContext = nullptr;
};

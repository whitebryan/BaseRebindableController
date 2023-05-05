#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KeybindsSaveFile.generated.h"

UCLASS()
class BASECONTROLLER_API UKeybindsSaveFile : public USaveGame
{
	GENERATED_BODY()

public: 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TMap<FName, FKey> inputMap;
};
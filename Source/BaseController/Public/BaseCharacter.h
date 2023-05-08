// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UCLASS()
class BASECONTROLLER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UUserWidget* menuWidget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Basic controller functions
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void MoveAction(FVector2D Value);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void CameraAction(FVector2D Value);
	UFUNCTION(BlueprintCallable, Category = "Movement")
	virtual void JumpAction(bool isJumping);
	UFUNCTION(BlueprintCallable, Category = "UI")
	virtual void MenuAction();
	UFUNCTION(BlueprintImplementableEvent)
	void toggleMenu();


};

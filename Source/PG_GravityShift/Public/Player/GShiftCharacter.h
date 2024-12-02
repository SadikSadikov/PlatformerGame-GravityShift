// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GShiftCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(Abstract)
class PG_GRAVITYSHIFT_API AGShiftCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AGShiftCharacter();

	/* Perform Postion adjustments */
	virtual void Tick(float DeltaTime) override;

	/* Setup input bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Player Pawn Initialization */
	virtual void PostInitializeComponents() override;

	/* Used to make Pawn jump : override to handle additional jump input functionality */
	virtual void CheckJumpInput(float DeltaTime) override;

	/* Notify from movement about hitting an obstacle while running */
	virtual void MoveBlockedBy(const FHitResult& Impact) override;

	/* Play end of round if game hase finished with character in mid air */
	virtual void Landed(const FHitResult& Hit) override;

protected:

	virtual void BeginPlay() override;

private:

	/* Input */

	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, Category = "Input|Context")
	TObjectPtr<UInputMappingContext> BaseContext;

	UPROPERTY(EditAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> BaseAction;




};

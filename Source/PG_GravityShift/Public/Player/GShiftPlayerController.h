// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GShiftPlayerController.generated.h"

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AGShiftPlayerController(const FObjectInitializer& ObjectInitializer);

	/* begin Actor Interface */

	/* After all game elements are created */
	virtual void PostInitializeComponents() override;

	/* end Acotr Interface */

	/* Try starting game */

	UFUNCTION(BlueprintCallable, Category = "Game")
	void TryStartingGame(bool& bIsStarted);

	/* Toggle InGameMenu Handler*/
	void OnToggleInGameMenu();

protected:
	

	/* begin PlayerController Interface */

	virtual void SetupInputComponent() override;

	/* end PlayerController Interface */

	virtual void BeginPlay() override;
	

	


	
};

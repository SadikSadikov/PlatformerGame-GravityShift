// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GShiftPlayerController.h"
#include "Player/GShiftPlayerCameraManager.h"

AGShiftPlayerController::AGShiftPlayerController(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = AGShiftPlayerCameraManager::StaticClass();
	
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
}

void AGShiftPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGShiftPlayerController::TryStartingGame(bool& bIsStarted)
{
}

void AGShiftPlayerController::OnToggleInGameMenu()
{
}

void AGShiftPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}

void AGShiftPlayerController::BeginPlay()
{
	Super::BeginPlay();
}


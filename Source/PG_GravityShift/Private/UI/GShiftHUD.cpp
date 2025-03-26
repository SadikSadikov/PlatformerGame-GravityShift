// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GShiftHUD.h"
#include "UI/GShiftOverlay.h"

void AGShiftHUD::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		if (PlayerController && GShiftOverlayClass)
		{
			GShiftOverlay = CreateWidget<UGShiftOverlay>(PlayerController, GShiftOverlayClass);
			GShiftOverlay->AddToViewport();
		}
	}
}

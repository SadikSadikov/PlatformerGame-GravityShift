// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GShiftHUD.generated.h"

class UGShiftOverlay;

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftHUD : public AHUD
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGShiftOverlay> GShiftOverlayClass;

	UPROPERTY()
	TObjectPtr<UGShiftOverlay> GShiftOverlay;

public:

	FORCEINLINE UGShiftOverlay* GetGShiftOverlay() { return GShiftOverlay; }
	
};

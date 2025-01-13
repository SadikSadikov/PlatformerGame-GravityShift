// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "GShiftPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class PG_GRAVITYSHIFT_API AGShiftPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	AGShiftPlayerCameraManager(const FObjectInitializer& FObjectInitializer);

	/* Sets new value of FixedCameraOffsetZ */
	UFUNCTION(Blueprintable, Category="Game|Player")
	void SetFixedCameraOffsetZ(float InOffset);

	/* Sets new value of CurrentZoomAlpha <0, 1> */
	UFUNCTION(BlueprintCallable, Category="Game|Player")
	void SetCameraZoom(float ZoomAlpha);

	/* Gets Current value of CurrentZoomAlpha */
	UFUNCTION(BlueprintCallable, Category="Game|Player")
	float GetCameraZoom() const;

protected:

	/* APlayerCameraManager Interface*/

	// Handle Camera Updates
	virtual void UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime) override;

	/* end APlayerCameraManager Interface */

	/* Calculates camera Z axis offset dependent on player pawn movement */
	float CalcCameraOffsetZ(float DeltaTime);

private:

	/* Fixed maximal camera distance from player pawn, used for zoom */
	FVector MaxCameraZoomOffset;

	/* Fixed minimal camera distance from player pawn, used for zoom */
	FVector MinCameraZoomOffset;

	/* Interpolation speed  for changing camera Z Axis offset */
	float CameraOffsetInterpSpeed;

	/* Fixed Camera Z axis offset from player pawn */
	float FixedCameraOffsetZ;

	/* Fixed rotation of the camera relative to Player Pawn */
	FRotator CameraFixedRotation;

	/* Current value of zoom < 0, 1 > ( 0 means MinCameraZoomOffset will be used, 1 mean MaxCameraZoomOffset will be used) */
	float CurrentZoomAlpha;

	/* Currently used camera Z axis offset */
	float CurrentCameraOffsetZ;

	/* Z Axis offset camera is supposed to achieve */
	float DesiredCameraOffsetZ;

	
};

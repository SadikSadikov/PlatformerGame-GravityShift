// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GShiftCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PG_GRAVITYSHIFT_API UGShiftCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:

	UGShiftCharacterMovementComponent(const FObjectInitializer& FObjectInitializer);

	/* Stop Slide when falling */
	virtual void StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc) override;

	/* Stop Movement and save current speed with obstacle modifier */
	void PauseMovementForObstacleHit();

	/* Stop Movement and save current speed with ledge crab modifier */
	void PauseMovementForLedgeCrab();

	/* Restore movement and saved speed */
	void RestoreMovement();
	
	/* Returns true when pawn is sliding */
	UFUNCTION(BlueprintCallable)
	bool IsSliding() const;

	/* Attempts to end slide move - fails if collisions above pawn don't allow it */
	void TryToEndSlide();

protected:

	/* Update Slide */
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	/* Forces Pawn to Start Sliding */
	void StartSlide();


	/* Calculates OutVelocity which is new Velocity for Pawn during slide */
	void CalcSlideVelocity(FVector& OutVelocity) const;

	/* While Pawn is Sliding calculates new Value of CurrentSlideVelocityReduction */
	void CalcCurrentSlideVelocityReduction(float DeltaTime);
 

	/* Changes pawn height to Slide Height and adjusts Pawn Collision */
	void SetSlideCollisionHeight();
	
	/*
	 * Restores pawn height to default after slide, if collisions above pawn allow that
	 * returns true if height change succeeded, false otherwise
	 */
	bool RestoreCollisionHeightAfterSlide();



private:

	/* Speed multiplayer after hitting obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float ModSpeedObstacleHit;

	/* Speed multiplayer after ledge grab */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float ModSpeedLedgeCrab;

	/* Value by which speed wil be reduced during slide */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float SlideVelocityReduction;

	/* Maximal speed Pawn can slide with */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MaxSlideSpeed;

	/* Minimal speed Pawn can slide with */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MinSlideSpeed;

	/* Height of Pawn While Sliding */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float SlideHeight;

	/* Offset Value, by which relative location of pawn mesh needs to be changed, when pawn is sliding*/
	FVector SlideMeshRelativeLocationOffset;

	/* Value by which sliding pawn speed is currently being reduced */
	float CurrentSlideVelocityReduction;

	/* Saved Modified value of speed to restore after animation finish */
	float SaveSpeed;

	/* True when pawn is sliding */
	uint32 bInSlide : 1;

	/* true if pawn needs to use SlideMeshRelativeLocationOffset while sliding */
	uint32 bWantsSlideMeshRelativeLocationOffset : 1;
	
};



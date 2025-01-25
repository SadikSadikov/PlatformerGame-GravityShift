// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HLSLTypeAliases.h"
#include "GameFramework/Character.h"
#include "GShiftCharacter.generated.h"

class AGShiftClimbMarker;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS(Abstract)
class PG_GRAVITYSHIFT_API AGShiftCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	AGShiftCharacter(const FObjectInitializer& ObjectInitializer);

	/* Perform Position adjustments */
	virtual void Tick(float DeltaTime) override;

	/* Setup input bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Player Pawn Initialization */
	virtual void PostInitializeComponents() override;

	/* Used to make Pawn jump : override to handle additional jump input functionality */
	virtual void CheckJumpInput(float DeltaTime) override;

	/* Start Jumping */
	virtual void Jump() override;

	/* Stop Jumping */
	virtual void StopJumping() override;

	/* Notify from movement about hitting an obstacle while running */
	virtual void MoveBlockedBy(const FHitResult& Impact) override;

	/* Play end of round if game hase finished with character in midair */
	virtual void Landed(const FHitResult& Hit) override;

	/* Notify from movement about Movement Mode changed, used for Cayote time */
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;

	/* Try playing end of round animation */
	void OnRoundFinished();

	/* Handle Effects when slide is finished */
	void PlaySlideFinished();

	/* Gets CameraHeightChangeThreshold value */
	float GetCameraHeightChangeThreshold();

	bool WantsToSlide() const;

protected:

	virtual void BeginPlay() override;

private:

	/* Input */

	void CharacterMove(const FInputActionValue& Value);

	void Slide();

	void StopSliding();

	void Dash();

	UPROPERTY(EditAnywhere, Category = "Input|Context")
	TObjectPtr<UInputMappingContext> BaseContext;

	UPROPERTY(EditAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> SlideAction;

	UPROPERTY(EditAnywhere, Category = "Input|Action")
	TObjectPtr<UInputAction> DashAction;
	
	/*
	* Camera is fixed to the ground, even when player jumps.
	* But, if player jumps higher tha this threshold, camera will start to follow.
	 */

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float CameraHeightChangeThreshold;
	
	/* Animation for winning game */
	UPROPERTY(EditDefaultsOnly, Category= "Animation")
	TObjectPtr<UAnimMontage> WonMontage;

	/* Animation for losing game */
	UPROPERTY(EditDefaultsOnly, Category= "Animation")
	TObjectPtr<UAnimMontage> LostMontage;

	/* Animation for running into an obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitWallMontage;
	
	/* Minimal speed for pawn to play hit wall animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float MinSpeedForHittingWal;

	/* Animation for climbing over small obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbOverSmallMontage;

	/* Height of small obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float ClimbOverSmallHeight;

	/* Animation for climbing over mid-obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbOverMidMontage;

	/* Height of mid-obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float ClimbOverMidHeight;

	/* Animation for climbing over big obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbOverBigMontage;

	/* Height of big obstacle */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float ClimbOverBigHeight;

	/* Animation for Climbing to ledge */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbLedgeMontage;

	/* Root offset in climb ledge animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	FVector ClimbLedgeRootOffset;

	/* Grab point offset along X axis in climb ledge animation */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	float ClimbLedgeGrabOffsetX;
	
	/* Mesh translation user for position adjustments */
	FVector AnimPositionAdjustment;

	/* Play End of round animation */
	void PlayRoundFinished();

	/* True when player is holding Slide Button */
	uint32 bPressedSlide : 1;
	
	/* Audio component playing looped slide sound */
	UPROPERTY()
	TObjectPtr<UAudioComponent> SlideAudioComponent;

	/* Handle for Efficient management of ClimbOverObstacle timer */
	FTimerHandle TimerHandle_ClimbOverObstacle;

	/* Handle for efficient management of ResumeMovement timer */
	FTimerHandle TimerHandle_ResumeMovement;

	/* Climb marker ( or to be exact its mesh component - the movable part) we are climbing to */
	UPROPERTY()
	UStaticMeshComponent* ClimbToMarker;

	/* Location of Climb Marker we are climbing to */
	FVector ClimbToMarkerLocation;

	/* Position Pawn in ledge and play animation with position adjustment */
	void ClimbToLedge(const AGShiftClimbMarker* MoveToMarker);
	
	/* Determine obstacle height type and play animation */
	void ClimbOverObstacle();

	/* Restore Pawn's Movement state */
	void ResumeMovement();

	// Cayote Jump
	bool bCanCoyoteJump = false;

	bool bHasJumped = false;

	void DisableCayoteJump();

	FTimerHandle TimerHandle_CayoteTime;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float CayoteTimeDuration = 0.2f;

	// Wall Slide
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bWallSlide;
	
	UPROPERTY(BlueprintReadOnly, Category = "Animation", meta = (AllowPrivateAccess = "true"))
	bool bWallSlideRight;

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float WallSlideInterpSpeed = 900.f;

	// Dash

	bool bCanDash = true;

	void ReactivateDash();

	

	




};




#include "Player/GShiftCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/AudioComponent.h"
#include "Player/GShiftCharacterMovementComponent.h"
#include "Game/GShiftGameModeBase.h"
#include "PG_GravityShift/PG_GravityShift.h"
#include "Actor/GShiftClimbMarker.h"
#include "Components/CapsuleComponent.h"

#include "PG_GravityShift/PrintString.h"

AGShiftCharacter::AGShiftCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UGShiftCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	MinSpeedForHittingWal = 200.f;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}


void AGShiftCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(GetController())->GetLocalPlayer()))
	{
		if (BaseContext)
		{
			Subsystem->AddMappingContext(BaseContext, 0);
		}
		
	}
	
}

void AGShiftCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Setting Initial rotation
	SetActorRotation(FRotator(0.f, 0.f, 0.f));
}

void AGShiftCharacter::Tick(float DeltaTime)
{
	// Decrease anim position adjustment
	if (!AnimPositionAdjustment.IsNearlyZero())
	{
		AnimPositionAdjustment = FMath::VInterpConstantTo(AnimPositionAdjustment, FVector::ZeroVector, DeltaTime, 400.f);
		GetMesh()->SetRelativeLocation(GetBaseTranslationOffset() + AnimPositionAdjustment);
	}

	
	Super::Tick(DeltaTime);

}

void AGShiftCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction && SlideAction && MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGShiftCharacter::CharacterMove);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGShiftCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGShiftCharacter::StopJumping);
			EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &AGShiftCharacter::Slide);
			EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Completed, this, &AGShiftCharacter::StopSliding);

		}
		
	}



}

void AGShiftCharacter::CheckJumpInput(float DeltaTime)
{
	if (bPressedJump)
	{
		UGShiftCharacterMovementComponent* MovementComponent = Cast<UGShiftCharacterMovementComponent>(GetCharacterMovement());
		if (MovementComponent && MovementComponent->IsSliding())
		{
			MovementComponent->TryToEndSlide();
			return;
		}
	}
	
	Super::CheckJumpInput(DeltaTime);
}

void AGShiftCharacter::Jump()
{
	bPressedJump = true;
	print("Jump is Pressed");
	Super::Jump();
	
}

void AGShiftCharacter::StopJumping()
{
	bPressedJump = false;
	print("Jump is Released");
	Super::StopJumping();
}

void AGShiftCharacter::CharacterMove(const FInputActionValue& Value)
{
	const FVector2D InputAxisValue = Value.Get<FVector2D>();

	AddMovementInput(GetActorForwardVector(), InputAxisValue.Y);
}

void AGShiftCharacter::Slide()
{
	bPressedSlide = true;
	print("Sliding is Pressed");
}

void AGShiftCharacter::StopSliding()
{
	bPressedSlide = false;
	print("Sliding is Released");
}

void AGShiftCharacter::MoveBlockedBy(const FHitResult& Impact)
{
	const float ForwardDot = FVector::DotProduct(Impact.Normal, FVector::ForwardVector);

	if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_None)
	{
		UE_LOG(LogGS, Log, TEXT("Collision with %s, normal=(%f, %f, %f), dot= %f, %s"),
			*GetNameSafe(Impact.GetActor()),
			Impact.Normal.X, Impact.Normal.Y, Impact.Normal.Z,
			ForwardDot,
			*GetCharacterMovement()->GetMovementName())
	}

	if (GetCharacterMovement()->MovementMode == MOVE_Walking && ForwardDot < -0.9f)
	{
		UGShiftCharacterMovementComponent* MovComp = Cast<UGShiftCharacterMovementComponent>(GetCharacterMovement());
		const float Speed = FMath::Abs(FVector::DotProduct(MovComp->Velocity, FVector::ForwardVector));
		// if running or sliding play bump reaction and jump over obstacle

		float Duration = 0.01f;

		if (Speed > MinSpeedForHittingWal)
		{
			Duration = PlayAnimMontage(HitWallMontage);
		}
		GetWorldTimerManager().SetTimer(TimerHandle_ClimbOverObstacle, this, &AGShiftCharacter::ClimbOverObstacle,
			Duration, false);

		MovComp->PauseMovementForObstacleHit();
	}
	else if (GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		// If in midair: try climbing to hit marker
		if (AGShiftClimbMarker* ClimbMarker = Cast<AGShiftClimbMarker>(Impact.GetActor()))
		{
			ClimbToLedge(ClimbMarker);
		}
		
		UGShiftCharacterMovementComponent* MovComp = Cast<UGShiftCharacterMovementComponent>(GetCharacterMovement());
		MovComp->PauseMovementForLedgeCrab();
		
	}

	
}

void AGShiftCharacter::ClimbToLedge(const AGShiftClimbMarker* MoveToMarker)
{
	print("Climbing to Ledge");
	
	ClimbToMarker = MoveToMarker ? MoveToMarker->FindComponentByClass<UStaticMeshComponent>() : nullptr;
	ClimbToMarkerLocation = ClimbToMarker ? ClimbToMarker->GetComponentLocation() : FVector::ZeroVector;

	// Place on top left corner of marker, but preserve current Y coordinate
	const FBox MarkerBox = MoveToMarker->GetMesh()->Bounds.GetBox();
	const FVector DesiredPosition(MarkerBox.Min.X, GetActorLocation().Y, MarkerBox.Max.Z);

	// Climbing to Ledge:
	// - Pawn is placed on top of ledge (using ClimbLedgeGrabOffsetX to offset from grab point) immediately
	// - AnimPositionAdjustment modifiers mesh relative location to smooth transition
	//   (Mesh starts roughly at the same position, additional offset quickly decreased to zero in Tick)

	const FVector StartPosition = GetActorLocation();
	FVector AdjustedPosition = DesiredPosition;
	AdjustedPosition.X += (ClimbLedgeGrabOffsetX * GetMesh()->GetRelativeScale3D().X) - GetBaseTranslationOffset().X;
	AdjustedPosition.Z += GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	TeleportTo(AdjustedPosition, GetActorRotation(), false, true);

	AnimPositionAdjustment = StartPosition - (GetActorLocation() - (ClimbLedgeRootOffset * GetMesh()->GetRelativeScale3D()));
	GetMesh()->SetRelativeLocation(GetBaseTranslationOffset() + AnimPositionAdjustment);

	if (ClimbLedgeMontage)
	{
		const float Duration = PlayAnimMontage(ClimbLedgeMontage);
		GetWorldTimerManager().SetTimer(TimerHandle_ResumeMovement,
			this, &AGShiftCharacter::ResumeMovement, Duration - 0.1f, false);
	}
	
	
}

void AGShiftCharacter::ClimbOverObstacle()
{
	print("Playing Climb over obstacle Animation");

	// Climbing over obstacle:
	// - There are three animations matching with three types of predefined obstacle heights
	// - Pawn is moved using root motion, ending up on top of obstacle as animation ends

	const FVector ForwardDir = GetActorForwardVector();
	const FVector TraceStart = GetActorLocation() + ForwardDir * 150.f + FVector(0.f, 0.f, 1.f) *
	(GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + 150.f);

	const FVector TraceEnd = TraceStart + FVector(0.f, 0.f, -1.f) * 500.f;

	FCollisionQueryParams TraceParams(NAME_None, FCollisionQueryParams::GetUnknownStatId(), true);
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, TraceParams);

	if (Hit.bBlockingHit)
	{
		const FVector DestPosition = Hit.ImpactPoint + FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		const float ZDiff = DestPosition.Z - GetActorLocation().Z;
		UE_LOG(LogGS, Log, TEXT("Climb over obstacle, Z difference: %f (%s)"), ZDiff,
			(ZDiff < ClimbOverMidHeight) ? TEXT("Small") : TEXT("Big"));

		UAnimMontage* Montage = (ZDiff < ClimbOverMidHeight) ? ClimbOverSmallMontage :
			(ZDiff < ClimbOverBigHeight) ? ClimbOverMidMontage : ClimbOverBigMontage;

		// Set flaying mode since it needs z changes. If Walking or Falling, we won't be able to apply Z Changes
		// This Gets reset in the ResumeMovement

		if (Montage)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			SetActorEnableCollision(false);
			const float Duration = PlayAnimMontage(Montage);
			GetWorldTimerManager().SetTimer(TimerHandle_ResumeMovement, this,
				&AGShiftCharacter::ResumeMovement, Duration - 0.1f, false);
		}

	}
	else
	{
		// Shouldn't happen
		ResumeMovement();
	}
	
}

void AGShiftCharacter::ResumeMovement()
{
	SetActorEnableCollision(true);

	// Restore Movement state and saved speed
	UGShiftCharacterMovementComponent* MovComp = Cast<UGShiftCharacterMovementComponent>(GetCharacterMovement());
	MovComp->RestoreMovement();

	ClimbToMarker = nullptr;
}

void AGShiftCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	AGShiftGameModeBase* MyGame = GetWorld()->GetAuthGameMode<AGShiftGameModeBase>();
	if (MyGame && MyGame->GetGameState() == EGameState::Finished)
	{
		PlayRoundFinished();
	}
}

void AGShiftCharacter::OnRoundFinished()
{
	
}

void AGShiftCharacter::PlaySlideFinished()
{
	if (SlideAudioComponent)
	{
		SlideAudioComponent->Stop();
		SlideAudioComponent = nullptr;
	}
}

float AGShiftCharacter::GetCameraHeightChangeThreshold()
{
	return CameraHeightChangeThreshold;
}

bool AGShiftCharacter::WantsToSlide() const
{
	return bPressedSlide;
}

void AGShiftCharacter::PlayRoundFinished()
{
	AGShiftGameModeBase* MyGame = GetWorld()->GetAuthGameMode<AGShiftGameModeBase>();

	const bool bWon = MyGame && MyGame->IsRoundWon();

	PlayAnimMontage(bWon ? WonMontage : LostMontage);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}




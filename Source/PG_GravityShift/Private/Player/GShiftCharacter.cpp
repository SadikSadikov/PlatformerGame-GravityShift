#include "Player/GShiftCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/AudioComponent.h"
#include "Player/GShiftCharacterMovementComponent.h"
#include "Game/GShiftGameModeBase.h"
#include "PG_GravityShift/PG_GravityShift.h"
#include "Actor/GShiftClimbMarker.h"
#include "Components/CapsuleComponent.h"
#include "GShiftComponents/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "PG_GravityShift/PrintString.h"
#include "UI/GShiftHUD.h"
#include "UI/GShiftOverlay.h"

AGShiftCharacter::AGShiftCharacter(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer.SetDefaultSubobjectClass<UGShiftCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	Tags.Add(FName("Player"));

	JumpMaxCount = 2;
	JumpMaxHoldTime = 0.5;
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
	Super::Tick(DeltaTime);
	
	// Decrease anim position adjustment
	if (!AnimPositionAdjustment.IsNearlyZero())
	{
		AnimPositionAdjustment = FMath::VInterpConstantTo(AnimPositionAdjustment, FVector::ZeroVector, DeltaTime, 400.f);
		GetMesh()->SetRelativeLocation(GetBaseTranslationOffset() + AnimPositionAdjustment);
	}
	if (ClimbToMarker)
	{
		// Correction in case climb marker is moving
		const FVector AdjustDelta = ClimbToMarker->GetComponentLocation() - ClimbToMarkerLocation;
		if (!AdjustDelta.IsNearlyZero())
		{
			SetActorLocation(GetActorLocation() + AdjustDelta, false);
			ClimbToMarkerLocation += AdjustDelta;
		}
	}

	
	if (GetCharacterMovement()->IsFalling())
	{
		FHitResult HitForward;
		TArray<AActor*> HitActors;
		UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorForwardVector() * 150.f + GetActorLocation(),
			TraceTypeQuery1, false, HitActors, EDrawDebugTrace::None, HitForward, true);

		if (HitForward.bBlockingHit)
		{
			bWallSlide = true;
			bWallSlideRight = false;

			GetCharacterMovement()->Velocity = FMath::VInterpConstantTo(GetCharacterMovement()->Velocity, FVector::ZeroVector,
				DeltaTime, WallSlideInterpSpeed);

		}
		else
		{
			FHitResult HitBackward;
			UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorForwardVector() * -150.f + GetActorLocation(),
				TraceTypeQuery1, false, HitActors, EDrawDebugTrace::None, HitBackward, true);

			if (HitBackward.bBlockingHit)
			{
				bWallSlide = true;
				bWallSlideRight = true;

				GetCharacterMovement()->Velocity = FMath::VInterpConstantTo(GetCharacterMovement()->Velocity, FVector::ZeroVector,
					DeltaTime, WallSlideInterpSpeed);

			}
			else
			{
				bWallSlide = false;
			}
	
		}
	}
	else
	{
		bWallSlide = false;
	}
	

}

void AGShiftCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (JumpAction && SlideAction && MoveAction && DashAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGShiftCharacter::CharacterMove);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGShiftCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AGShiftCharacter::StopJumping);
			EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &AGShiftCharacter::Slide);
			EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Completed, this, &AGShiftCharacter::StopSliding);
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AGShiftCharacter::Dash);


		}
		
	}



}

void AGShiftCharacter::CheckJumpInput(float DeltaTime)
{
	if (bPressedJump)
	{
		bHasJumped = true;
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
	
	if (bCanCoyoteJump && !bHasJumped)
	{
		LaunchCharacter(FVector(0.f, 0.f, GetCharacterMovement()->JumpZVelocity),
			false, false);

		JumpCurrentCount++;
	}

	if (bWallSlide)
	{
		if (!bWallSlideRight)
		{
			// TODO:: Make names for 500.f and FVector(0.f, 0.f, 1000.f)
			LaunchCharacter(GetActorForwardVector() * -500.f + FVector(0.f, 0.f, 1000.f), false, false);

		}
		else
		{
			LaunchCharacter(GetActorForwardVector() * 500.f + FVector(0.f, 0.f, 1000.f), false, false);

		}
	}
	

		
	
	
}

void AGShiftCharacter::StopJumping()
{
	bPressedJump = false;
	print("Jump is Released");
	Super::StopJumping();


}

void AGShiftCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Falling)
	{
		bCanCoyoteJump = true;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CayoteTime, this, &AGShiftCharacter::DisableCayoteJump,
			CayoteTimeDuration, false);
		
		
	}

	
}

void AGShiftCharacter::DisableCayoteJump()
{
	bCanCoyoteJump = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_CayoteTime);
}

void AGShiftCharacter::ReactivateDash()
{
	bCanDash = true;
}

void AGShiftCharacter::CharacterMove(const FInputActionValue& Value)
{
	const FVector2D InputAxisValue = Value.Get<FVector2D>();

	FVector ActorForward = GetOwner()->GetActorForwardVector();

	// If Attacking don`t change Actor Rotation 
	if (InputAxisValue.Y != 0.f && !CombatComponent->bIsAttacking)
	{
		float TargetYaw = InputAxisValue.Y > 0.f ? 0.f : 180.f;

		FRotator NewRotation(0.f, TargetYaw, 0.f);
		
		SetActorRotation(NewRotation);
		
	}
	

	AddMovementInput(ActorForward, InputAxisValue.Y);
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

void AGShiftCharacter::Dash()
{
	if (bCanDash)
	{

		// TODO:: Put name on this value 1200.f and InRate bellow for the timer
		LaunchCharacter(GetActorForwardVector() * 2000.f, false, false);
		bCanDash = false;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AGShiftCharacter::ReactivateDash,
			2.f);
	}
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

			UGShiftCharacterMovementComponent* MovComp = Cast<UGShiftCharacterMovementComponent>(GetCharacterMovement());
			MovComp->PauseMovementForLedgeCrab();
		}
		
		
		
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
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

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

	bHasJumped = false;
	DisableCayoteJump();
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

void AGShiftCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	if (CombatComponent)
	{
		CombatComponent->OnHealthChangedDelegate.Broadcast(CombatComponent->Health + Damage,CombatComponent->Health);
	}
	

	if (CombatComponent->bIsDead)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		DisableInput(PlayerController);
	}
	
}

void AGShiftCharacter::PlayRoundFinished()
{
	AGShiftGameModeBase* MyGame = GetWorld()->GetAuthGameMode<AGShiftGameModeBase>();

	const bool bWon = MyGame && MyGame->IsRoundWon();

	PlayAnimMontage(bWon ? WonMontage : LostMontage);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}




#include "Player/GShiftCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/AudioComponent.h"
#include "Player/GShiftCharacterMovementComponent.h"
#include "Game/GShiftGameModeBase.h"
#include "PG_GravityShift/PG_GravityShift.h"
#include "Animation/AnimInstance.h"

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

	/*if (GetCharacterMovement()->MovementMode != EMovementMode::MOVE_None)
	{
		UE_LOG(LogGS, Log, TEXT("Collision with %s, normal=(%f, %f, %f), dot= %f, %s"),
			*GetNameSafe(Impact.GetActor()),
			Impact.Normal.X, Impact.Normal.Y, Impact.Normal.Z,
			ForwardDot,
			*GetCharacterMovement()->GetMovementName())
	}*/

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
	// TODO:: Make Hitting obstacle in air

	
}

void AGShiftCharacter::ClimbOverObstacle()
{
	print("Playing Climb over obstacle Animation");
	ResumeMovement();
}

void AGShiftCharacter::ResumeMovement()
{
	SetActorEnableCollision(true);

	// Restore Movement state and saved speed
	UGShiftCharacterMovementComponent* MovComp = Cast<UGShiftCharacterMovementComponent>(GetCharacterMovement());
	MovComp->RestoreMovement();

	// ClimbToMarker = nullptr
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




// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GShiftCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "Player/GShiftCharacter.h"

#include "DrawDebugHelpers.h"
#include "PG_GravityShift/PrintString.h"

UGShiftCharacterMovementComponent::UGShiftCharacterMovementComponent(const FObjectInitializer& FObjectInitializer)
	: Super(FObjectInitializer)
{
	MaxAcceleration = 200.0f;
	BrakingDecelerationWalking = MaxAcceleration;
	MaxWalkSpeed = 900.f;

	SlideVelocityReduction = 30.f;
	SlideHeight = 60.f;
	SlideMeshRelativeLocationOffset = FVector(0.f, 0.f, 34.f);
	bWantsSlideMeshRelativeLocationOffset = true;
	MinSlideSpeed = 200.f;
	MaxSlideSpeed = MaxWalkSpeed + MinSlideSpeed;

	ModSpeedObstacleHit = 0.f;
	ModSpeedLedgeCrab = 0.8f;
}

void UGShiftCharacterMovementComponent::StartFalling(int32 Iterations, float remainingTime, float timeTick,
	const FVector& Delta, const FVector& subLoc)
{
	Super::StartFalling(Iterations, remainingTime, timeTick, Delta, subLoc);

	if (MovementMode == MOVE_Falling && IsFalling())
	{
		TryToEndSlide();
	}
}

void UGShiftCharacterMovementComponent::PauseMovementForObstacleHit()
{

	print("Hitting a Obstacle");
	
	SaveSpeed = Velocity.Size() * ModSpeedObstacleHit;
	StopMovementImmediately();
	DisableMovement();
	TryToEndSlide();
	
}

void UGShiftCharacterMovementComponent::PauseMovementForLedgeCrab()
{
	print("Hitting a Ledge Crab");
	
	SaveSpeed = Velocity.Size() * ModSpeedLedgeCrab;
	StopMovementImmediately();
	DisableMovement();
	TryToEndSlide();
}

void UGShiftCharacterMovementComponent::RestoreMovement()
{
	SetMovementMode(MOVE_Walking);

	if (SaveSpeed > 0)
	{
		Velocity = PawnOwner->GetActorForwardVector() * SaveSpeed;
	}

   

}

bool UGShiftCharacterMovementComponent::IsSliding() const
{
	return bInSlide;
}

void UGShiftCharacterMovementComponent::TryToEndSlide()
{
	// End slide if collisions allow
	if (bInSlide)
	{
		if (RestoreCollisionHeightAfterSlide())
		{
			bInSlide = false;
			if (AGShiftCharacter* MyOwner = Cast<AGShiftCharacter>(GetOwner()))
			{
				MyOwner->PlaySlideFinished();
			}
			
		}
	}
}

void UGShiftCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{

	if (AGShiftCharacter* MyOwner = Cast<AGShiftCharacter>(GetOwner()))
	{
		const bool bWantsToSlide = MyOwner->WantsToSlide();
		if (IsSliding())
		{
			CalcCurrentSlideVelocityReduction(deltaTime);
			CalcSlideVelocity(Velocity);

			const float CurrentSpeedSquare = Velocity.SizeSquared();
			if (CurrentSpeedSquare <= FMath::Square(MinSlideSpeed))
			{
				// Slide has min speed - try to end it
				TryToEndSlide();
			}
		}
		else if (bWantsToSlide)
		{
			if (!IsFalling() &&
				Velocity.SizeSquared() >= FMath::Square(MinSlideSpeed * 2.f)) // Make sure Pawn has some Velocity
			{
				StartSlide();
			}
		}
		
	}
	
	Super::PhysWalking(deltaTime, Iterations);
}

void UGShiftCharacterMovementComponent::StartSlide()
{
	if (!bInSlide)
	{
		bInSlide = true;
		CurrentSlideVelocityReduction = 0.f;
		SetSlideCollisionHeight();
	}
}

void UGShiftCharacterMovementComponent::CalcCurrentSlideVelocityReduction(float DeltaTime)
{
	float ReductionCoeficent = 0.f;

	const float FloorDotVelocity = FVector::DotProduct(CurrentFloor.HitResult.ImpactNormal,
		Velocity.GetSafeNormal());

	
	const bool bNeedsSlopedAdjustment = (FloorDotVelocity != 0.f);

	if (bNeedsSlopedAdjustment)
	{
		const float Multiplier = 1.f + FMath::Abs<float>(FloorDotVelocity);
		if (FloorDotVelocity > 0.f)
		{
			ReductionCoeficent += SlideVelocityReduction * Multiplier; // Increasing speed when sliding down a slope
		}
		else
		{
			ReductionCoeficent -= SlideVelocityReduction * Multiplier; // Reducing speed when sliding up a slope 
		}
	}
	else
	{
		ReductionCoeficent -= SlideVelocityReduction; // Reducing speed on flat ground
	}

	float TimeDilation = GetWorld()->GetWorldSettings()->GetEffectiveTimeDilation();
	CurrentSlideVelocityReduction += (ReductionCoeficent * TimeDilation * DeltaTime);
}

void UGShiftCharacterMovementComponent::CalcSlideVelocity(FVector& OutVelocity) const
{
	const FVector VelocityDirection = Velocity.GetSafeNormal();
	FVector NewVelocity = Velocity + CurrentSlideVelocityReduction * VelocityDirection;

	const float NewSpeedSquare = NewVelocity.SizeSquared();
	if (NewSpeedSquare > FMath::Square(MaxSlideSpeed))
	{
		NewVelocity = VelocityDirection * MaxSlideSpeed;
	}
	else if (NewSpeedSquare < FMath::Square(MinSlideSpeed))
	{
		NewVelocity = VelocityDirection * MinSlideSpeed;
	}

	OutVelocity = NewVelocity;
}

void UGShiftCharacterMovementComponent::SetSlideCollisionHeight()
{
	if (!CharacterOwner || SlideHeight <= 0.f) return;

	// Do not perform if collision is already at desired size
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == SlideHeight) return;

	// Change Collision size to new Value
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius(),
		SlideHeight);

	// Applying correction to PawnOwner mesh relative location
	if (bWantsSlideMeshRelativeLocationOffset)
	{
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		const FVector Correction = DefaultCharacter->GetMesh()->GetRelativeLocation() + SlideMeshRelativeLocationOffset;
		CharacterOwner->GetMesh()->SetRelativeLocation(Correction);
	}
}

bool UGShiftCharacterMovementComponent::RestoreCollisionHeightAfterSlide()
{
	if (!GetCharacterOwner() || !UpdatedPrimitive) return false;

	ACharacter* DefCharacter = GetCharacterOwner()->GetClass()->GetDefaultObject<ACharacter>();
	const float DefHalfHeight = DefCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float DefRadius = DefCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();


	// Do not perform if collision is already at desired size
	if (GetCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefHalfHeight)
	{
		return true;
	}

	const float HeightAdjust = DefHalfHeight - GetCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const FVector NewLocation = GetCharacterOwner()->GetActorLocation() + FVector(0, 0, HeightAdjust);

	// Check if there is enough space for default capsule size
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(FinishSlide), false, GetCharacterOwner());
	FCollisionResponseParams ResponseParam;

	InitCollisionParams(TraceParams, ResponseParam);
	const bool bBlocked = GetWorld()->OverlapBlockingTestByChannel(NewLocation, FQuat::Identity, UpdatedPrimitive->GetCollisionObjectType(),
		FCollisionShape::MakeCapsule(DefRadius, DefHalfHeight), TraceParams);

	if (bBlocked) return false;

	/* Restore Capsule size and move up to adjusted location */
	GetCharacterOwner()->TeleportTo(NewLocation, GetCharacterOwner()->GetActorRotation(), false, true);
	GetCharacterOwner()->GetCapsuleComponent()->SetCapsuleSize(DefRadius, DefHalfHeight);

	/* Restoring original Pawn Owner mesh relative location */
	if (bWantsSlideMeshRelativeLocationOffset)
	{
		GetCharacterOwner()->GetMesh()->SetRelativeLocation(DefCharacter->GetMesh()->GetRelativeLocation());
	}

	return true;
}

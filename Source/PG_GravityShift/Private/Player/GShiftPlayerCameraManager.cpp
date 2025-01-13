// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GShiftPlayerCameraManager.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Player/GShiftCharacter.h"

AGShiftPlayerCameraManager::AGShiftPlayerCameraManager(const FObjectInitializer& FObjectInitializer)
 : Super(FObjectInitializer)
{
	MinCameraZoomOffset = FVector(240.f, 600.f, 0.f);
	MaxCameraZoomOffset = MinCameraZoomOffset * 4;
	CurrentZoomAlpha = 0.1f;

	DesiredCameraOffsetZ = 0.f;
	CurrentCameraOffsetZ = 0.f;
	CameraOffsetInterpSpeed = 5.f;

	CameraFixedRotation = FRotator(0, -90, 0);
	FixedCameraOffsetZ = 130.f;
}

void AGShiftPlayerCameraManager::SetFixedCameraOffsetZ(float InOffset)
{
	FixedCameraOffsetZ = InOffset;
}

void AGShiftPlayerCameraManager::SetCameraZoom(float ZoomAlpha)
{
	CurrentZoomAlpha = FMath::Clamp(ZoomAlpha, 0.f, 1.f);
}

float AGShiftPlayerCameraManager::GetCameraZoom() const
{
	return CurrentZoomAlpha;
}

void AGShiftPlayerCameraManager::UpdateViewTargetInternal(FTViewTarget& OutVT, float DeltaTime)
{
	FVector ViewLoc;
	FRotator ViewRot;
	OutVT.Target->GetActorEyesViewPoint(ViewLoc, ViewRot);
	ViewLoc.Z = CalcCameraOffsetZ(DeltaTime);
	ViewLoc.Z += FixedCameraOffsetZ;

	FVector CurrentCameraZoomOffset = MinCameraZoomOffset + CurrentZoomAlpha * (MaxCameraZoomOffset - MinCameraZoomOffset);
	OutVT.POV.Location = ViewLoc + CurrentCameraZoomOffset;
	OutVT.POV.Rotation = CameraFixedRotation;
}

float AGShiftPlayerCameraManager::CalcCameraOffsetZ(float DeltaTime)
{
	if (AGShiftCharacter* MyPawn = PCOwner ? Cast<AGShiftCharacter>(PCOwner->GetPawn()) : nullptr)
	{
		float LocZ = MyPawn->GetActorLocation().Z;
		if (MyPawn->GetCharacterMovement() && MyPawn->GetCharacterMovement()->IsFalling())
		{
			if (LocZ < DesiredCameraOffsetZ)
			{
				DesiredCameraOffsetZ = LocZ;
			}
			else if (LocZ > DesiredCameraOffsetZ + MyPawn->GetCameraHeightChangeThreshold())
			{
				DesiredCameraOffsetZ = LocZ;
			}
		}
		else
		{
			DesiredCameraOffsetZ = LocZ;
		}
		
	}

	if (CurrentCameraOffsetZ != DesiredCameraOffsetZ)
	{
		CurrentCameraOffsetZ = FMath::FInterpTo(CurrentCameraOffsetZ, DesiredCameraOffsetZ, DeltaTime, CameraOffsetInterpSpeed);
	}

	return CurrentCameraOffsetZ;

	
}

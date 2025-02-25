// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GShiftBaseCharacter.h"
#include "GShiftComponents/CombatComponent.h"

// Sets default values
AGShiftBaseCharacter::AGShiftBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;


	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

}

TArray<FCombatMontage> AGShiftBaseCharacter::GetAttackMontages_Implementation()
{
	return CombatComponent->AttackMontages;
}

void AGShiftBaseCharacter::GetCombatSocketLocation_Implementation(FVector& OutSocketLocation,
	const ECombatSocket& InSocket)
{
	if (CombatComponent->SocketNames.IsEmpty())
	{
		OutSocketLocation = FVector::ZeroVector;
		return;
	}

	for (TTuple<ECombatSocket, FName> SocketName : CombatComponent->SocketNames)
	{
		if (SocketName.Key == InSocket)
		{
			OutSocketLocation = GetMesh()->GetSocketLocation(SocketName.Value);
		}
	}
}

void AGShiftBaseCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatComponent->CombatTarget = InCombatTarget;
}

AActor* AGShiftBaseCharacter::GetCombatTarget_Implementation() const
{
	return CombatComponent->CombatTarget;
}

void AGShiftBaseCharacter::BroadcastMontageEventReceivedDelegate_Implementation(const EWeaponType& WeaponType)
{
	CombatComponent->OnMontageEventDelegate.Broadcast(WeaponType);
}


// Called when the game starts or when spawned
void AGShiftBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGShiftBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGShiftBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


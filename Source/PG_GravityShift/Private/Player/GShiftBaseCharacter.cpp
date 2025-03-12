// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GShiftBaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GShiftComponents/CombatComponent.h"
#include "Kismet/GameplayStatics.h"

#include "PG_GravityShift/PrintString.h"

// Sets default values
AGShiftBaseCharacter::AGShiftBaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;


	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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
			if (Weapon && InSocket == ECombatSocket::ECS_Weapon)
			{
				OutSocketLocation = Weapon->GetSocketLocation(SocketName.Value);
			}
			else
			{
				OutSocketLocation = GetMesh()->GetSocketLocation(SocketName.Value);
			}
			
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

	if (GetMesh() && GetMesh()->GetSkeletalMeshAsset())
	{
		int Index = 0;
		for (auto Value : GetMesh()->GetMaterials())
		{
			if (UMaterialInstanceDynamic* DynamicInstance = UMaterialInstanceDynamic::Create(Value, this))
			{
				GetMesh()->SetMaterial(Index, DynamicInstance);
			}
			++Index;
		}
	}

	OnTakeAnyDamage.AddDynamic(this, &AGShiftBaseCharacter::ReceiveDamage);
	if (CombatComponent)
	{
		CombatComponent->OnHitReactDelegate.AddDynamic(this, &AGShiftBaseCharacter::ReceiveHitReactDelegate);
	}
	
}

void AGShiftBaseCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{

	if (GetMesh() && GetMesh()->GetSkeletalMeshAsset())
	{
		for (auto Value : GetMesh()->GetMaterials())
		{
			if (UMaterialInstanceDynamic* DynamicInstance = Cast<UMaterialInstanceDynamic>(Value))
			{
				float Time = UGameplayStatics::GetTimeSeconds(this);
				
				DynamicInstance->SetScalarParameterValue(FName("HitStartTime"), Time);
				DynamicInstance->SetScalarParameterValue(FName("HitDuration"), HitDuration);
				DynamicInstance->SetVectorParameterValue(FName("HitFlashColor"), HitFlashColor);
			}
		}
	}
	
	CombatComponent->Health = FMath::Clamp(CombatComponent->Health - Damage, 0.f, CombatComponent->MaxHealth);

	CombatComponent->HitReact();

	if (CombatComponent->Health <= 0.f)
	{
		CombatComponent->Death();
		
	}
	

	printf("%s Health is - %f", *GetName(), CombatComponent->Health);
}

void AGShiftBaseCharacter::ReceiveHitReactDelegate(bool bHitReacting)
{
}


void AGShiftBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




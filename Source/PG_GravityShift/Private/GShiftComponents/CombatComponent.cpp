#include "GShiftComponents/CombatComponent.h"

#include "Actor/GShiftProjectile.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/Character.h"

UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::SpawnProjectile(const FVector& ProjectileTargetLocation, const ECombatSocket& Socket)
{

	// Projectile Spawn Location
	FVector SocketLocation;
	ICombatInterface::Execute_GetCombatSocketLocation(GetOwner(), SocketLocation, Socket);

	// Rotation of Projectile towards enemy
	FRotator ProjectileRotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	FTransform ProjectileTransform;
	ProjectileTransform.SetLocation(SocketLocation);
	ProjectileTransform.SetRotation(ProjectileRotation.Quaternion());

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = GetOwner()->GetInstigator();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	GetWorld()->SpawnActor<AGShiftProjectile>(ProjectileClass,
		ProjectileTransform, SpawnParameters);

	

	
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UCombatComponent::RangedAttack(EInputType InputType)
{
	if (!GetOwner()->Implements<UCombatInterface>()) return;

	AActor* Target = ICombatInterface::Execute_GetCombatTarget(GetOwner());
	if (!Target) return;

	// Update Motion Warping, Definition of this function is in Blueprint
	ICombatInterface::Execute_UpdateFacingTarget(GetOwner(), Target->GetActorLocation());
	
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	
	TArray<FCombatMontage> Montages = ICombatInterface::Execute_GetAttackMontages(GetOwner());
	if (Montages.IsEmpty()) return;
	
	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	for (FCombatMontage CombatMontage : Montages)
	{

		if (InputType == CombatMontage.InputType)
		{
			if (CombatMontage.CombatType == ECombatType::ECT_Ranged)
			{
				AnimInstance->Montage_Play(CombatMontage.Montage);

				if (!OnMontageEventDelegate.IsBound())
				{
					OnMontageEventDelegate.AddLambda([this, Target, CombatMontage](EWeaponType WeaponType)
				{
					if (WeaponType == CombatMontage.WeaponType)
					{
						SpawnProjectile(Target->GetActorLocation(), CombatMontage.Socket);

						OnMontageEventDelegate.Clear();
					}
							
						
				});
				}
			}
		}
	}
	
}
	


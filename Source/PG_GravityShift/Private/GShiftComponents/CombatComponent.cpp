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

void UCombatComponent::SpawnProjectile(const FVector& ProjectileTargetLocation, const ECombatSocketType& SocketType)
{

	// Projectile Spawn Location
	FVector SocketLocation;
	ICombatInterface::Execute_GetCombatSocketLocation(GetOwner(), SocketLocation, SocketType);

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

void UCombatComponent::MontageEventReceived(ECombatType CombatType)
{
}


void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

void UCombatComponent::RangedAttack()
{
	if (GetOwner()->Implements<UCombatInterface>())
	{

		if (AActor* Target = ICombatInterface::Execute_GetCombatTarget(GetOwner()))
		{
			// Update Motion Warping, Definition of this function is in Blueprint
			ICombatInterface::Execute_UpdateFacingTarget(GetOwner(), Target->GetActorLocation());

			if (ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner()))
			{
				UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
				
				TArray<FCombatMontage> Montages = ICombatInterface::Execute_GetAttackMontages(GetOwner());
				if (!Montages.IsEmpty())
				{
					// TODO:: Make func for pick random montage from Attack Montages 
					AnimInstance->Montage_Play(Montages[0].Montage);

					// TODO:: Make Event in montage when is ready for spawning the projectile
					// for now this will be work

					OnMontageEventDelegate.AddLambda([this, Target, Montages](ECombatType CombatType)
					{
						if (CombatType == Montages[0].Type)
						{
							SpawnProjectile(Target->GetActorLocation(), Montages[0].SocketType);
						}
						
					});
					
				}
				
			}

			
		}
	}
}


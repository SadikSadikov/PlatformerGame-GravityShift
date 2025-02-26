#include "GShiftComponents/CombatComponent.h"

#include "Actor/GShiftProjectile.h"
#include "Engine/OverlapResult.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/Character.h"

#include "PG_GravityShift/PrintString.h"

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

void UCombatComponent::Punch(const ECombatSocket& Socket)
{
	FVector SocketLocation;
	ICombatInterface::Execute_GetCombatSocketLocation(GetOwner(), SocketLocation, Socket);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	TArray<AActor*> OverlappingActors;
	GetLivePlayersWithinRadius(ActorsToIgnore, 45.f, SocketLocation, OverlappingActors);

	for (AActor* OverlapedActor : OverlappingActors)
	{
		// TODO:: Make Damage
		printf("Overlapped Actor - %s", *OverlapedActor->GetName());
	}

	
}

void UCombatComponent::GetLivePlayersWithinRadius(TArray<AActor*> ActorsToIgnore, float Radius, FVector SphereOrigin,
	TArray<AActor*>& OutOverlappingActors)
{
	if (GetWorld())
	{
		FCollisionQueryParams Params;
		Params.AddIgnoredActors(ActorsToIgnore);
		
		TArray<FOverlapResult> OverlapResults;
		GetWorld()->OverlapMultiByObjectType(OverlapResults, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams::InitType::AllDynamicObjects,\
			FCollisionShape::MakeSphere(Radius), Params);

		for (FOverlapResult& OverlapResult : OverlapResults)
		{
			// TODO:: Check if overlapped actor implement ICombatInterface and IsNotDead
			OutOverlappingActors.AddUnique(OverlapResult.GetActor());
		}
	}
}




void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentComboCount > 0 && CurrentComboCount < ComboMaxCount)
	{
		CurrentPunchTime += DeltaTime;

		if (CurrentPunchTime > ComboThreshold && !bIsComboTimerResetting) 
		{
			bIsComboTimerResetting = true;
			ResetComboWithDelay();
			print("Resetting Combo");
		}
	}

	
}

void UCombatComponent::RangedAttack(EInputType InputType)
{
	if (!GetOwner()->Implements<UCombatInterface>()) return;
	if (!CombatTarget) return;

	// Update Motion Warping, Definition of this function is in Blueprint
	ICombatInterface::Execute_UpdateFacingTarget(GetOwner(), CombatTarget->GetActorLocation());
	
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	
	if (AttackMontages.IsEmpty()) return;
	
	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	for (FCombatMontage CombatMontage : AttackMontages)
	{

		if (InputType == CombatMontage.InputType)
		{
			if (CombatMontage.CombatType == ECombatType::ECT_Ranged)
			{
				AnimInstance->Montage_Play(CombatMontage.Montage);

				if (!OnMontageEventDelegate.IsBound())
				{
					OnMontageEventDelegate.AddLambda([this, CombatMontage](EWeaponType WeaponType)
				{
					if (WeaponType == CombatMontage.WeaponType)
					{
						SpawnProjectile(CombatTarget->GetActorLocation(), CombatMontage.Socket);

						OnMontageEventDelegate.Clear();
					}
							
						
				});
				}
			}
		}
	}
	
}

void UCombatComponent::ResetCombo()
{
	CurrentComboCount = 0;
	CurrentPunchTime = 0.f;
}

void UCombatComponent::ResetComboWithDelay()
{

	if (!GetWorld()->GetTimerManager().IsTimerActive(ComboResetTimer))
	{
		GetWorld()->GetTimerManager().SetTimer(ComboResetTimer, this, &UCombatComponent::ResetCombo, ComboResetTime);
	}
	
}

void UCombatComponent::MeleeAttack(EInputType InputType)
{
	if (!GetOwner()->Implements<UCombatInterface>()) return;
	if (!CombatTarget) return;

	// Update Motion Warping, Definition of this function is in Blueprint
	ICombatInterface::Execute_UpdateFacingTarget(GetOwner(), CombatTarget->GetActorLocation());

	if (AttackMontages.IsEmpty()) return;

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	
	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	// Combo Attack, if you have
	
	if (CurrentComboCount < ComboMaxCount && CurrentPunchTime < ComboThreshold)
	{
		if (bMeleeAttacking) return;
		printf("Current Combo Count: %d", CurrentComboCount);
		CurrentComboCount++;
		CurrentPunchTime = 0.f;
		bMeleeAttacking = true;

		for (FCombatMontage CombatMontage : AttackMontages)
		{
			if (InputType == CombatMontage.InputType)
			{
				if (CombatMontage.CombatType == ECombatType::ECT_Melee)
				{
					AnimInstance->Montage_Play(CombatMontage.Montage);
					FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), CurrentComboCount));
					AnimInstance->Montage_JumpToSection(SectionName, CombatMontage.Montage);
					
					if (!OnMontageEventDelegate.IsBound())
					{
						OnMontageEventDelegate.AddLambda([this, CombatMontage](EWeaponType WeaponType)
					{
						if (WeaponType == CombatMontage.WeaponType)
						{
							
							bMeleeAttacking = false;
								
							
							Punch(CombatMontage.Socket);
							OnMontageEventDelegate.Clear();
						}
							
						
					});
					}
				}
			}
		}
		
	}
	else
	{
		ResetComboWithDelay();
	}

	
}


	


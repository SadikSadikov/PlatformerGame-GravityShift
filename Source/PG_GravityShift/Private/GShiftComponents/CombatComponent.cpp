#include "GShiftComponents/CombatComponent.h"

#include "Actor/GShiftProjectile.h"
#include "Engine/OverlapResult.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

#include "PG_GravityShift/PrintString.h"
#include "UniversalObjectLocators/AnimInstanceLocatorFragment.h"


UCombatComponent::UCombatComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;
	
}

float UCombatComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCombatComponent::SpawnProjectile(FVector ProjectileTargetLocation, const ECombatSocket& Socket)
{

	// Projectile Spawn Location
	FVector SocketLocation;
	ICombatInterface::Execute_GetCombatSocketLocation(GetOwner(), SocketLocation, Socket);
	SocketLocation.Y = 0.f;

	if (FireParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, FireParticle, SocketLocation);
	}

	if (FireSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, FireSound, SocketLocation);
	}
	

	
	
	
	// Rotation of Projectile towards enemy
	if (ProjectileTargetLocation.IsNearlyZero())
	{
		ProjectileTargetLocation = SocketLocation + (GetOwner()->GetActorForwardVector() * 1000.f);
	}
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

	DrawDebugSphere(GetWorld(), SocketLocation, 45, 20, FColor::Yellow, true);
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());
	TArray<AActor*> OverlappingActors;
	GetLivePlayersWithinRadius(ActorsToIgnore, 45.f, SocketLocation, OverlappingActors);

	for (AActor* OverlapedActor : OverlappingActors)
	{
		UGameplayStatics::ApplyDamage(OverlapedActor, MeleeDamageAmount, GetOwner()->GetInstigatorController(),
			GetOwner(), UDamageType::StaticClass());

		// TODO:: Call GetHit from ICombatInterface
		
		printf("Overlapped Actor - %s", *OverlapedActor->GetName());
	}

	
}

void UCombatComponent::HitReact()
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;

	if (bIsHitReacting) return;

	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
	if (AnimInstance && !HitReactMontage) return;

	CurrentMontage = HitReactMontage;
	bIsHitReacting = true;
	OnHitReactDelegate.Broadcast(bIsHitReacting);

	if (!AnimInstance->OnMontageEnded.IsAlreadyBound(this, &UCombatComponent::OnMontageEnded))
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &UCombatComponent::OnMontageEnded);
	}
	
	AnimInstance->Montage_Play(HitReactMontage);

	
}

void UCombatComponent::Death()
{
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	
	bIsDead = true;
	
	CharacterOwner->GetCharacterMovement()->DisableMovement();
	CharacterOwner->GetCharacterMovement()->StopMovementImmediately();

	CharacterOwner->GetMesh()->SetSimulatePhysics(true);
	CharacterOwner->GetMesh()->SetEnableGravity(true);
	CharacterOwner->GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	CharacterOwner->GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	CharacterOwner->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
}

void UCombatComponent::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (CurrentMontage != Montage) return;
	
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
	//
	bIsHitReacting = false;
	OnHitReactDelegate.Broadcast(bIsHitReacting);

	AnimInstance->OnMontageEnded.RemoveDynamic(this, &UCombatComponent::OnMontageEnded);
	
}

void UCombatComponent::OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (CurrentMontage != Montage) return;
	if (bInterrupted)
	{
		bIsAttacking = false;
		OnMontageEventDelegate.Clear();
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	AnimInstance->OnMontageBlendingOut.RemoveDynamic(this, &UCombatComponent::OnAttackMontageBlendingOut);
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
			ResetCombo(EWeaponType::EWT_Hand);
			print("Resetting Combo");
		}
	}

	
}

void UCombatComponent::RangedAttack(EInputType InputType)
{
	if (bIsAttacking || bIsRocketAttacking) return;
	if (!GetOwner()->Implements<UCombatInterface>()) return;

	FVector CombatLocation = FVector::ZeroVector;
	
	if (CombatTarget)
	{
		// Update Motion Warping, Definition of this function is in Blueprint
		ICombatInterface::Execute_UpdateFacingTarget(GetOwner(), CombatTarget->GetActorLocation());

		CombatLocation = CombatTarget->GetActorLocation();
	}
	
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
				
				GetWorld()->GetTimerManager().SetTimer(RocketAttackTimerHandle, this, &UCombatComponent::RocketAttackFinished, RocketCooldown);
				
				OnAbilityUsedDelegate.Broadcast(CombatMontage.WeaponType, RocketCooldown);
				
				bIsAttacking = true;
				bIsRocketAttacking = true;
				
				if (bNotUseMontageRangedAttack)
				{
					SpawnProjectile(CombatLocation, *CombatMontage.Sockets.Find(1));
					bIsAttacking = false;
					return;
				}
				CurrentMontage = CombatMontage.Montage;
				AnimInstance->Montage_Play(CombatMontage.Montage);

				if (!AnimInstance->OnMontageBlendingOut.IsAlreadyBound(this, &UCombatComponent::OnAttackMontageBlendingOut))
				{
					AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UCombatComponent::OnAttackMontageBlendingOut);
				}

				if (!OnMontageEventDelegate.IsBound())
				{
					OnMontageEventDelegate.AddLambda([this, CombatMontage, CombatLocation](EWeaponType WeaponType)
				{
					if (WeaponType == CombatMontage.WeaponType)
					{
						SpawnProjectile(CombatLocation, *CombatMontage.Sockets.Find(1));
						bIsAttacking = false;
						OnMontageEventDelegate.Clear();
					}
							
						
				});
				}
			}
		}
	}
	
}

void UCombatComponent::RocketAttackFinished()
{
	bIsRocketAttacking = false;
	GetWorld()->GetTimerManager().ClearTimer(RocketAttackTimerHandle);
}

void UCombatComponent::ResetCombo(EWeaponType Type)
{
	GetWorld()->GetTimerManager().SetTimer(RocketAttackTimerHandle, this, &UCombatComponent::RocketAttackFinished, RocketCooldown);

	OnAbilityUsedDelegate.Broadcast(Type, PunchCooldown);
	CurrentComboCount = 0;
	CurrentPunchTime = 0.f;
}

void UCombatComponent::ResetComboWithDelay()
{

	if (!GetWorld()->GetTimerManager().IsTimerActive(ComboResetTimer))
	{
		//GetWorld()->GetTimerManager().SetTimer(ComboResetTimer, this, &UCombatComponent::ResetCombo, ComboResetTime);
	}
	
}



void UCombatComponent::MeleeAttack(EInputType InputType)
{
	if (bIsAttacking || bIsRocketAttacking) return;
	if (!GetOwner()->Implements<UCombatInterface>()) return;
	
	if (CombatTarget)
	{
		// Update Motion Warping, Definition of this function is in Blueprint
		ICombatInterface::Execute_UpdateFacingTarget(GetOwner(), CombatTarget->GetActorLocation());
	}

	if (AttackMontages.IsEmpty()) return;

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;
	
	UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();

	// Combo Attack, if you have
	
	if (CurrentComboCount < ComboMaxCount && CurrentPunchTime < ComboThreshold)
	{
		
		printf("Current Combo Count: %d", CurrentComboCount);
		CurrentComboCount++;
		CurrentPunchTime = 0.f;
		
		for (FCombatMontage CombatMontage : AttackMontages)
		{
			if (InputType == CombatMontage.InputType)
			{
				if (CombatMontage.CombatType == ECombatType::ECT_Melee)
				{
				
					OnAbilityUsedWithComboDelegate.Broadcast(CombatMontage.WeaponType, ComboThreshold, CurrentComboCount, ComboMaxCount);
					
					bIsAttacking = true;

					CurrentMontage = CombatMontage.Montage;
					AnimInstance->Montage_Play(CombatMontage.Montage);
					FName SectionName = FName(*FString::Printf(TEXT("Attack%d"), CurrentComboCount));
					AnimInstance->Montage_JumpToSection(SectionName, CombatMontage.Montage);

					if (!AnimInstance->OnMontageBlendingOut.IsAlreadyBound(this, &UCombatComponent::OnAttackMontageBlendingOut))
					{
						AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UCombatComponent::OnAttackMontageBlendingOut);
					}
					
					if (!OnMontageEventDelegate.IsBound())
					{
						OnMontageEventDelegate.AddLambda([this, CombatMontage](EWeaponType WeaponType)
					{
						if (WeaponType == CombatMontage.WeaponType)
						{
							// CurrentComboCount Start from 1
							if (CombatMontage.Sockets.Find(CurrentComboCount))
							{
								Punch(*CombatMontage.Sockets.Find(CurrentComboCount));
							}
							else
							{
								printf("You don`t have enough Melee Combo Section in Montage...");
							}
							

							bIsAttacking = false;
							
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
		ResetCombo(EWeaponType::EWT_Hand);
	}

	
}

void UCombatComponent::PunchAttackFinished()
{
	bIsRocketAttacking = false;
	GetWorld()->GetTimerManager().ClearTimer(RocketAttackTimerHandle);
}


	


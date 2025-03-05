// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/GShiftEnemy.h"
#include "AI/GShiftAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GShiftComponents/CombatComponent.h"


AGShiftEnemy::AGShiftEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Tags.Add(FName("Enemy"));
}

void AGShiftEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	GShiftAIController = Cast<AGShiftAIController>(NewController);
	if (GShiftAIController && BehaviorTree && BehaviorTree->BlackboardAsset)
	{
		GShiftAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		GShiftAIController->RunBehaviorTree(BehaviorTree);
		GShiftAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"),
			ICombatInterface::Execute_GetAttackMontages(this)[0].CombatType == ECombatType::ECT_Ranged );
		
	}

	
}

void AGShiftEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}

void AGShiftEnemy::ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	if (CombatComponent->bIsDead && GShiftAIController)
	{
		GShiftAIController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"), CombatComponent->bIsDead );
		SetLifeSpan(DeathLifeSpan);
	}
	
	
}

void AGShiftEnemy::ReceiveHitReactDelegate(bool bHitReacting)
{
	Super::ReceiveHitReactDelegate(bHitReacting);

	if (GShiftAIController)
	{
		GShiftAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), bHitReacting);
	}
	
	
}

void AGShiftEnemy::AimOffset(float DeltaTime)
{

	if (!GetController()) return;
	if (!CombatComponent->CombatTarget) return;

	const FVector Direction = CombatComponent->CombatTarget->GetActorLocation() - GetActorLocation();
	const FRotator LookAtRotation = Direction.Rotation();

	AO_Pitch = LookAtRotation.Pitch;
}

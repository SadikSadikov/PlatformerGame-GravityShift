// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/EnemyInterface.h"
#include "Player/GShiftBaseCharacter.h"
#include "GShiftEnemy.generated.h"

class UBehaviorTree;
class AGShiftAIController;
/**
 * 
 */
UCLASS()
class PG_GRAVITYSHIFT_API AGShiftEnemy : public AGShiftBaseCharacter, public IEnemyInterface
{
	GENERATED_BODY()

public:

	AGShiftEnemy(const FObjectInitializer& ObjectInitializer);

	virtual void PossessedBy(AController* NewController) override;

	virtual void Tick(float DeltaTime) override;

	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;

protected:


	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY()
	TObjectPtr<AGShiftAIController> GShiftAIController;

	virtual void ReceiveHitReactDelegate(bool bHitReacting) override;

private:

	float AO_Pitch;

	void AimOffset(float DeltaTime);

public:	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	
	
};



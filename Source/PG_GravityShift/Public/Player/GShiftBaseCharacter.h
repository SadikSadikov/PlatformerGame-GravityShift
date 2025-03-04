// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "GShiftBaseCharacter.generated.h"

class UCombatComponent;

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftBaseCharacter : public ACharacter, public ICombatInterface
{
	GENERATED_BODY()

public:
	
	AGShiftBaseCharacter(const FObjectInitializer& ObjectInitializer);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* Combat Interface */

	virtual TArray<FCombatMontage> GetAttackMontages_Implementation() override;

	virtual void GetCombatSocketLocation_Implementation(FVector& OutSocketLocation, const ECombatSocket& InSocket) override; 

	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	
	virtual AActor* GetCombatTarget_Implementation() const override;

	virtual void BroadcastMontageEventReceivedDelegate_Implementation(const EWeaponType& WeaponType) override;
	
	/* end Combat Interface */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCombatComponent> CombatComponent;

private:



public:

	


};

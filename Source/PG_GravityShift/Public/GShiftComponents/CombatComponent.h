// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/CombatInterface.h"
#include "CombatComponent.generated.h"


class AGShiftProjectile;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMontageEventReceivedSignature, ECombatType /*CombatType*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PG_GRAVITYSHIFT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void RangedAttack();

	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<FCombatMontage> AttackMontages;

	/* Your character maybe contains more than one socket for combat Key is actual SocketName */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TMap<ECombatSocketType, FName> SocketNames;

	UPROPERTY(BlueprintReadWrite, Category = "Config")
	TObjectPtr<AActor> CombatTarget;
	
	FOnMontageEventReceivedSignature OnMontageEventDelegate;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& ProjectileTargetLocation, const ECombatSocketType& SocketType);

	UPROPERTY(EditAnywhere, Category = "RangedAttack")
	TSubclassOf<AGShiftProjectile> ProjectileClass;

private:
	
	UFUNCTION()
	void MontageEventReceived(ECombatType CombatType);

	
	
		
};

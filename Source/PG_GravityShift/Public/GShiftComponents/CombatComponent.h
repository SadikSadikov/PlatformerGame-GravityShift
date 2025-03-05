// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interaction/CombatInterface.h"
#include "CombatComponent.generated.h"


class AGShiftProjectile;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMontageEventReceivedSignature, EWeaponType /*WeaponType*/);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitReactSignature, bool, bHitReacting);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PG_GRAVITYSHIFT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void RangedAttack(EInputType InputType = EInputType::EIT_NONE);

	UFUNCTION(BlueprintCallable)
	void MeleeAttack(EInputType InputType = EInputType::EIT_NONE);

	void HitReact();

	void Death();
	
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TArray<FCombatMontage> AttackMontages;

	/* Your character maybe contains more than one socket for combat Key is actual SocketName */
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	TMap<ECombatSocket, FName> SocketNames;

	UPROPERTY(BlueprintReadWrite, Category = "Config")
	TObjectPtr<AActor> CombatTarget;

	FOnMontageEventReceivedSignature OnMontageEventDelegate;

	UPROPERTY(BlueprintReadOnly, BlueprintAssignable)
	FOnHitReactSignature OnHitReactDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Config")
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config|HitReact")
	bool bIsHitReacting = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config|HitReact")
	bool bIsDead = false;


protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(FVector ProjectileTargetLocation, const ECombatSocket& Socket);

	UFUNCTION(BlueprintCallable)
	void Punch(const ECombatSocket& Socket);

	UPROPERTY(EditAnywhere, Category = "Config|RangedAttack")
	TSubclassOf<AGShiftProjectile> ProjectileClass;

	// Hit React

	UPROPERTY(EditDefaultsOnly, Category = "Config|HitReact")
	TObjectPtr<UAnimMontage> HitReactMontage;

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnAttackMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	// Death

	UPROPERTY(EditDefaultsOnly, Category = "Config|HitReact")
	TObjectPtr<UAnimMontage> DeathMontage;

	UFUNCTION()
	void OnDeathAnimationBlendingOut(UAnimMontage* Montage, bool bInterrupted);


private:
	

	void GetLivePlayersWithinRadius(TArray<AActor*> ActorsToIgnore, float Radius, FVector SphereOrigin,  TArray<AActor*>& OutOverlappingActors);

	UPROPERTY(VisibleAnywhere, Category = "Config|Melee|Combo")
	int32 CurrentComboCount = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Melee|Combo")
	int32 ComboMaxCount = 3;

	UPROPERTY(VisibleAnywhere, Category = "Config|Melee|Combo")
	float CurrentPunchTime = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Melee|Combo")
	float ComboThreshold = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Melee|Combo")
	float ComboResetTime = 1.f;

	FTimerHandle ComboResetTimer;

	bool bIsComboTimerResetting = false;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Melee|Combo")
	float MeleeDamageAmount = 15.f;

	void ResetCombo();

	void ResetComboWithDelay();

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentMontage;
	

	
	
		
};

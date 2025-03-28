// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GShiftProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGShiftProjectile();

	virtual void Tick(float DeltaTime) override;

protected:
	
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	bool IsValidOverlap(AActor* OtherActor);

	void SpawnTrailSystem();

private:

	UPROPERTY(EditAnywhere, Category = "Config")
	float LifeSpan = 15.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UParticleSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UNiagaraSystem> TrailSystem;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UParticleSystem> Tracer;

	UPROPERTY()
	TObjectPtr<UParticleSystemComponent> TracerComponent;

	UPROPERTY(EditAnywhere, Category = "Config")
	float DamageAmount = 20.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bSpawnTrail = false;

	

	
	
	

};



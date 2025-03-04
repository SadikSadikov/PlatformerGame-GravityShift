// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GShiftProjectile.h"

#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

AGShiftProjectile::AGShiftProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SetRootComponent(SphereComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 550.f;
	ProjectileMovementComponent->MaxSpeed = 550.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;

	// Temporary solution for hitting other bullets or rockets
	Tags.Add(FName("Projectile"));

}


void AGShiftProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGShiftProjectile::OnSphereOverlap);

	if (LoopingSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	}
	

}

void AGShiftProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	if (ImpactSound && ImpactEffect)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, ImpactSound, GetActorLocation());
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}
	
	
	Super::Destroyed();
}

void AGShiftProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	UGameplayStatics::ApplyDamage(OtherActor, DamageAmount, GetOwner()->GetInstigatorController(),
			this, UDamageType::StaticClass());
	
	GEngine->AddOnScreenDebugMessage(1, 2.5f, FColor::Blue,
		FString::Printf(TEXT("Overlapped Actor is %s"), *OtherActor->GetName()));

	Destroy();
	
}

bool AGShiftProjectile::IsValidOverlap(AActor* OtherActor)
{
	
	if (GetInstigator() == OtherActor || OtherActor->ActorHasTag(FName("Projectile"))) return false;

	FName TargetTag;
	GetInstigator()->Tags.Contains(FName("Player")) ? TargetTag = FName("Enemy") : TargetTag = FName("Player");
	
	if (!OtherActor->Tags.Contains(TargetTag)) return false;

	return true;
}

// Called every frame
void AGShiftProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


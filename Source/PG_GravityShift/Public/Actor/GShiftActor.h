// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GShiftActor.generated.h"

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftActor : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGShiftActor();

protected:
	
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComp;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> MeshComp;
	

};

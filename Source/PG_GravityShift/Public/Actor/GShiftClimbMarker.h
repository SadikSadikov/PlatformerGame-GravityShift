// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GShiftClimbMarker.generated.h"

UCLASS(Blueprintable)
class PG_GRAVITYSHIFT_API AGShiftClimbMarker : public AActor
{
	GENERATED_BODY()
	
public:
	
	AGShiftClimbMarker();

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;

public:

	/* Returns mesh subobject */
	FORCEINLINE UStaticMeshComponent* GetMesh() const { return Mesh; }

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GShiftClimbMarker.h"

AGShiftClimbMarker::AGShiftClimbMarker()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClimbMesh"));
	Mesh->SetupAttachment(GetRootComponent());

}



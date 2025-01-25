// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GShiftActor.h"


AGShiftActor::AGShiftActor()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>("SceneComponent");
	SetRootComponent(SceneComp);
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	MeshComp->SetupAttachment(GetRootComponent());

}

void AGShiftActor::BeginPlay()
{
	Super::BeginPlay();
	
}



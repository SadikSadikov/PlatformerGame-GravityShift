// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/GShiftProps.h"

// Sets default values
AGShiftProps::AGShiftProps()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGShiftProps::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGShiftProps::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


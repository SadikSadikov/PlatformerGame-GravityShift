// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GShiftAIController.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftAIController : public AAIController
{
	GENERATED_BODY()

public:
	
	AGShiftAIController();

protected:

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
	
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAnimMontage;


UENUM(BlueprintType)
enum class ECombatSocketType : uint8
{
	ECST_NONE UMETA(DisplayName = "NONE"),
	ECST_WeaponSocket UMETA(DisplayName = "WeaponSocket"),
	ECST_RightHandSocket UMETA(DisplayName = "RightHandSocket"),
	ECST_LeftHandSocket UMETA(DisplayName = "LeftHandSocket")
};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
	ECT_NONE UMETA(DisplayName = "NONE"),
	ECT_Hand UMETA(DisplayName = "Hand"),
	ECT_Rocket UMETA(DisplayName = "Rocket"),
	ECT_Bullet UMETA(DisplayName = "Bullet")
};

USTRUCT(BlueprintType)
struct FCombatMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatSocketType SocketType = ECombatSocketType::ECST_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatType Type = ECombatType::ECT_NONE;
	
};

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PG_GRAVITYSHIFT_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FCombatMontage> GetAttackMontages();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetCombatSocketLocation(FVector& OutSocketLocation, const ECombatSocketType& InSocketType);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BroadcastMontageEventReceivedDelegate(const ECombatType& CombatType);
};

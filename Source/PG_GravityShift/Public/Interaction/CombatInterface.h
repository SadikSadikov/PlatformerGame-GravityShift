// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

class UAnimMontage;


UENUM(BlueprintType)
enum class ECombatSocket : uint8
{
	ECS_NONE UMETA(DisplayName = "NONE"),
	ECS_Weapon UMETA(DisplayName = "Weapon"),
	ECS_RightHand UMETA(DisplayName = "RightHand"),
	ECS_LeftHand UMETA(DisplayName = "LeftHand")
};

UENUM(BlueprintType)
enum class EInputType : uint8
{
	EIT_NONE UMETA(DisplayName = "NONE"),
	EIT_RMB UMETA(DisplayName = "RMB"),
	EIT_LMT UMETA(DisplayName = "LMB")

};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_NONE UMETA(DisplayName = "NONE"),
	EWT_Hand UMETA(DisplayName = "Hand"),
	EWT_Rocket UMETA(DisplayName = "Rocket")

};

UENUM(BlueprintType)
enum class ECombatType : uint8
{
	ECT_NONE UMETA(DisplayName = "NONE"),
	ECT_Ranged UMETA(DisplayName = "Ranged"),
	ECT_Melee UMETA(DisplayName = "Melee")

};

USTRUCT(BlueprintType)
struct FCombatMontage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatSocket Socket = ECombatSocket::ECS_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponType WeaponType = EWeaponType::EWT_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatType CombatType = ECombatType::ECT_NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EInputType InputType = EInputType::EIT_NONE;
	
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
	void GetCombatSocketLocation(FVector& OutSocketLocation, const ECombatSocket& InSocket);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BroadcastMontageEventReceivedDelegate(const EWeaponType& WeaponType);
};

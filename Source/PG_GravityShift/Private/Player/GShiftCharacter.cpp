#include "Player/GShiftCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "PG_GravityShift/PrintString.h"


AGShiftCharacter::AGShiftCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGShiftCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Cast<APlayerController>(GetController())->GetLocalPlayer()))
	{
		if (BaseContext)
		{
			Subsystem->AddMappingContext(BaseContext, 0);
		}
		
	}
	
}

void AGShiftCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGShiftCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (BaseAction)
		{
			EnhancedInputComponent->BindAction(BaseAction, ETriggerEvent::Triggered, this, &AGShiftCharacter::Move);
		}
		
	}



}

void AGShiftCharacter::Move(const FInputActionValue& Value)
{
	const bool bPressed = Value.Get<bool>();

	if (bPressed)
	{
		print("True");
	}
	else
	{
		print("False");
	}

}

void AGShiftCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Setting Initial rotation
	SetActorRotation(FRotator(0.f, 0.f, 0.f));
}

void AGShiftCharacter::CheckJumpInput(float DeltaTime)
{
}

void AGShiftCharacter::MoveBlockedBy(const FHitResult& Impact)
{
}

void AGShiftCharacter::Landed(const FHitResult& Hit)
{
}


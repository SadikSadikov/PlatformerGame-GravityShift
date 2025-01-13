// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GShiftGameModeBase.generated.h"

namespace EGameState
{
	enum Type
	{
		Intro,
		Waiting,
		Playing,
		Finished,
		Restarting,
	};
	
}

UCLASS()
class PG_GRAVITYSHIFT_API AGShiftGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	/* Get current state of game */
	EGameState::Type GetGameState() const;

	/* Returns true if round was won (Best Time) */
	bool IsRoundWon() const;

	/* Returns true if Round is in progress - player is still moving*/
	bool IsRoundInProgress() const;


private:

	/* True when round is in progress */
	EGameState::Type GameState;

	/* True if player won this round, false otherwise */
	uint32 bRoundWasWon : 1;

	
	
};

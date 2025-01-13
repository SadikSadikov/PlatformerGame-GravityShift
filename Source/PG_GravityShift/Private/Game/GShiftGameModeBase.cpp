// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GShiftGameModeBase.h"

EGameState::Type AGShiftGameModeBase::GetGameState() const
{
	return GameState;
}

bool AGShiftGameModeBase::IsRoundWon() const
{
	return bRoundWasWon;
}

bool AGShiftGameModeBase::IsRoundInProgress() const
{
	return GameState == EGameState::Playing;
}

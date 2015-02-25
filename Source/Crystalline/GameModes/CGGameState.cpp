// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerState.h"
#include "CGGameState.h"

ACGGameState::ACGGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RemainingTime = 0;
	PlayerPredicate = FComparePlayers();
}

void ACGGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGGameState, RemainingTime);
}

void ACGGameState::SortPlayers()
{
	// TODO this may not be the fastest way to do things, Noticable lag on kill confirm.
	PlayerArray.StableSort(PlayerPredicate);
}

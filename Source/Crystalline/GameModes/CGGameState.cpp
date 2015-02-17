// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerState.h"
#include "CGGameState.h"

ACGGameState::ACGGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	RemainingTime = 0;
}

void ACGGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGGameState, RemainingTime);
}

/**
void AGameState::GetPlayerRanks(PlayerRankMap& OutPlayerRanks)
{
	// TODO build ranks, map is unecessary.

}*/
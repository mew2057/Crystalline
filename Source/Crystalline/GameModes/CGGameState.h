// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "CGGameState.generated.h"

//typedef TMap<int32, TWeakObjectPtr<ACGPlayerState> > PlayerRankMap;
/** Predicate for player sorts. */
struct FComparePlayers
{
	FORCEINLINE bool operator()(const APlayerState& A, const APlayerState& B) const
	{
		return A.Score > B.Score;
	}
};
/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGGameState : public AGameState
{
	GENERATED_BODY()

public:
	ACGGameState(const FObjectInitializer& ObjectInitializer);

	void SortPlayers();

	/** Time left in the round, post game, pre game, etc.*/
	UPROPERTY(Transient, Replicated)
	int32 RemainingTime;
private:
	FComparePlayers PlayerPredicate;
};

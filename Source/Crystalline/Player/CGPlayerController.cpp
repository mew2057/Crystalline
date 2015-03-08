// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerController.h"

void ACGPlayerController::UnFreeze()
{
	ServerRestartPlayer();
	
	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		// Clear the scoreboard and prompt on respawn.
		HUD->SetScoreboardVisibility(false);
		HUD->SetPromptMessage(false); // XXX this is a bandaid! fix this!
	}
}


void ACGPlayerController::FailedToSpawnPawn()
{
	if (StateName == NAME_Inactive)
	{
		BeginInactiveState();
	}
	Super::FailedToSpawnPawn();
}


void ACGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("Scoreboard", IE_Pressed, this, &ACGPlayerController::OnShowScoreboard);
	InputComponent->BindAction("Scoreboard", IE_Released, this, &ACGPlayerController::OnHideScoreboard);
}

void ACGPlayerController::OnShowScoreboard()
{
	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->SetScoreboardVisibility(true);
	}
}

void ACGPlayerController::OnHideScoreboard()
{
	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->SetScoreboardVisibility(false);
	}
}

void ACGPlayerController::ClientGameEnded_Implementation(AActor* EndGameFocus, bool bIsWinner)
{
	Super::ClientGameEnded_Implementation(EndGameFocus, bIsWinner);

	// Tell player that they've won.

	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->SetScoreboardVisibility(true);

		//TODO prevent scoreboard hiding.
		if (bIsWinner)
		{
			HUD->SetEndGameMessage(true, "Game Over, You Win");
		}
		else
		{			
			// TODO *Winner Name* Wins.
			HUD->SetEndGameMessage(true, "Game Over, You Lost");
		}
	}
}

void ACGPlayerController::ClientGameStarted_Implementation()
{
	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->SetScoreboardVisibility(false);
		HUD->SetEndGameMessage(false);
	}
}

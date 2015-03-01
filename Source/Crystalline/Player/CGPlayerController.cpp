// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerController.h"

void ACGPlayerController::SpawnDefaultHUD()
{
	Super::SpawnDefaultHUD();
}


void ACGPlayerController::UnFreeze()
{
	ServerRestartPlayer();
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
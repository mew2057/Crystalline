// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerController.h"

void ACGPlayerController::SpawnDefaultHUD()
{
	Super::SpawnDefaultHUD();

	if (MyHUD)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawned HUD"));
	}

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



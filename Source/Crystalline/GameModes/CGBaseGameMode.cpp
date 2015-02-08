// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBaseGameMode.h"
#include "Player/CGCharacter.h"
#include "Player/CGPlayerController.h"
#include "GUI/CGPlayerHUD.h"



ACGBaseGameMode::ACGBaseGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Player/CGPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	PlayerControllerClass = ACGPlayerController::StaticClass();
	HUDClass = ACGPlayerHUD::StaticClass();
}

void ACGBaseGameMode::SetPlayerDefaults(APawn* PlayerPawn)
{
	Super::SetPlayerDefaults(PlayerPawn);

	ACGCharacter* CGCharacter = Cast<ACGCharacter>(PlayerPawn);

	if (CGCharacter != NULL)
	{
		CGCharacter->SpawnBaseInventory();
	}

}

/*
void ACGBaseGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	

	if (NewPlayer)
	{
		ACGCharacter* Player = Cast<ACGCharacter>(NewPlayer->GetPawn());

		if (Player)
		{
			Player->InitLocalValues();
		}
	}
}*/


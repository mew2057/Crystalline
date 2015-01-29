// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Crystalline.h"
#include "CrystallineGameMode.h"
#include "CrystallineHUD.h"
#include "Player/CrystallinePlayerController.h"
#include "Player/CrystallinePlayerMoveComponent.h"


ACrystallineGameMode::ACrystallineGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/Player/CrystallinePlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;


	//PlayerControllerClass = ACrystallinePlayerController::StaticClass();

	// use our custom HUD class
	HUDClass = ACrystallineHUD::StaticClass();
}

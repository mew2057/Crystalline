// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGPlayerController.h"
#include "Misc/CGSavedPlayerData.h"

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

void ACGPlayerController::PawnPendingDestroy(APawn * inPawn)
{
	Super::PawnPendingDestroy(inPawn);

	// TODO more spectator stuff here.
	ClientSetSpectatorCamera(inPawn->GetActorLocation(), inPawn->GetActorRotation());
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

void ACGPlayerController::OnDeathMessage(ACGPlayerState* Killer, ACGPlayerState* KilledPlayer, const UDamageType* DamageType)
{
	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->AddDialogKillMessage(Killer, KilledPlayer, DamageType);
	}
}


void ACGPlayerController::OnGameScoreMessage(int32 MessageIndex)
{
	ACGPlayerHUD* HUD = Cast<ACGPlayerHUD>(GetHUD());
	if (HUD)
	{
		HUD->AddDialogGameScoreMessage(MessageIndex);
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

	UCGSavedPlayerData* LoadGameInstance = Cast<UCGSavedPlayerData>(UGameplayStatics::CreateSaveGameObject(UCGSavedPlayerData::StaticClass()));
	LoadGameInstance = Cast<UCGSavedPlayerData>(UGameplayStatics::LoadGameFromSlot("SavedPlayerData", 0));

	// This needs to be vetted, there is NO GUARANTEE that either GEngine OR LoadGameInstance will have a value.
	if (LoadGameInstance && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, LoadGameInstance->PlayerName);
		if (LoadGameInstance->UsingIPAddress == true){
			FString PlayerNameToDisplay = LoadGameInstance->PlayerName;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, PlayerNameToDisplay);
			//if (GetWorld()->IsServer()){
			SetName(PlayerNameToDisplay);
			//}
		}
	}
}

void ACGPlayerController::ClientSetSpectatorCamera_Implementation(const FVector & Location, const FRotator & Rotation)
{
	SetInitialLocationAndRotation(Location, Rotation);
	SetViewTarget(this);
}


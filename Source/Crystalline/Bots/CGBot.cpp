// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Bots/CGBotController.h"
#include "CGBot.h"

ACGBot::ACGBot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AIControllerClass = ACGBotController::StaticClass();

	bUseControllerRotationYaw = true;
}

void ACGBot::FaceRotation(FRotator NewRotation, float DeltaTime)
{
	// Prevents the rotation from being instant (technique from framework game.
	FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 8.0f);

	Super::FaceRotation(CurrentRotation, DeltaTime);
}

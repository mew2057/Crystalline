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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/CGCharacter.h"
#include "CGBot.generated.h"

/**
 * Crystalline's base AI player pawn class. Overrides certain elements of the 
 * ACGCharacter to ensure the bots have a consistent execution. Adds a Behavior Tree to
 * control the bot.
 */
UCLASS()
class CRYSTALLINE_API ACGBot : public ACGCharacter
{
	GENERATED_BODY()
public:
	/** Defines all of the behavior for the AI.*/
	UPROPERTY(EditAnywhere, Category = Behavior)
	class UBehaviorTree* BotBehavior;

	ACGBot(const FObjectInitializer& ObjectInitializer);
	
	/**Interpolates the rotation of the bot to prevent aim hacks.*/
	virtual void FaceRotation(FRotator NewRotation, float DeltaTime) override;

	/*@return False, the bot is never first person.**/
	virtual bool IsFirstPerson() override;

};

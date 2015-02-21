// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Player/CGCharacter.h"
#include "CGBot.generated.h"

/**
 * 
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
	
	virtual void FaceRotation(FRotator NewRotation, float DeltaTime) override;

};

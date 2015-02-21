// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "CGBotController.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGBotController : public AAIController
{
	GENERATED_BODY()
private:
	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

public:

	ACGBotController(const FObjectInitializer& ObjectInitializer);

	virtual void Possess(class APawn* InPawn) override;

	/**Basically the AI controller's unfreeze.*/
	virtual void BeginInactiveState() override;


	void Respawn();

	UFUNCTION(BlueprintCallable, Category=Behavior)
	void SearchForEnemy();

	void SetEnemy(class APawn* InPawn);
	ACGCharacter* GetEnemy();

	UFUNCTION(BlueprintCallable, Category = Behavior)
	void ShootEnemy();

	/** Makes the bot look at the player.*/
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;

private:

	uint8 EnemyKeyID;

	uint8 DestinationKeyID;

	
	
};

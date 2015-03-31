// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "CGBotController.generated.h"

/**
 * Defines the AI Controller for Crystalline.
 */
UCLASS()
class CRYSTALLINE_API ACGBotController : public AAIController
{
	GENERATED_BODY()
private:
	/**The Brain of the Bots.*/
	UPROPERTY(transient)
	class UBlackboardComponent* BlackboardComp;

	/** The Decision maker of the Bots.*/
	UPROPERTY(transient)
	class UBehaviorTreeComponent* BehaviorComp;

public:

	ACGBotController(const FObjectInitializer& ObjectInitializer);

	/**
	 * Initializes the bot on Possession.
	 */
	virtual void Possess(class APawn* InPawn) override;

	/**
	 * The AI controller's unfreeze equivalent. Triggers the respawn logic.
	 */
	virtual void BeginInactiveState() override;

	/**
	 * Callback for the Respawn Handle. Restarts the bot.
	 */
	void Respawn();

	/**
	 * Finds the nearest enemy to the Bot, does not account for line of sight.
	 * XXX Does not have team support.
	 */
	UFUNCTION(BlueprintCallable, Category=Behavior)
	void SearchForEnemy();

	/**
	 * Sets the enemy in the Blackboard of the AI.
	 * @param InPawn The pawn for the new enemy.
	 */
	void SetEnemy(class APawn* InPawn);

	/**
	* Retrieves the enemy from the AI's Blackboard.
	* @return The enemy currently stored on the Blackboard, if the Blackboard is missing returns nullptr.
	*/
	ACGCharacter* GetEnemy();

	/**
	 * Shoots the enemy if the Bot has Line of Sight to the enemy currently in the
	 * Blackboard. Uses ACGCharacter's firing logic.
	 */
	UFUNCTION(BlueprintCallable, Category = Behavior)
	void ShootEnemy();

	/** Makes the bot look at the player.*/
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;

private:

	/** The Key for the current Enemy Pawn on the Blackboard.*/
	uint8 EnemyKeyID;

	/** The Key for the current Destination Vector on the Blackboard.*/
	uint8 DestinationKeyID;

	/**Timer Handle for the bot's respawn.*/
	FTimerHandle TimerHandle_Respawn;
};

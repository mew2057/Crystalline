// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "CGBTTask_FindPointNearEnemy.generated.h"

/**
 * Defines a Task for the Bot AIController. This Task finds a 
 * point that the bot can use for navigation in its behavior tree.
 */
UCLASS()
class CRYSTALLINE_API UCGBTTask_FindPointNearEnemy : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	
	/** Finds a point between the two actors.*/
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

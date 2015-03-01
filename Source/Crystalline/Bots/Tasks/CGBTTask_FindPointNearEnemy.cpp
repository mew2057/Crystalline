// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBTTask_FindPointNearEnemy.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"




EBTNodeResult::Type UCGBTTask_FindPointNearEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Verify the controller is valid, if not fail the task.
	ACGBotController* Controller = Cast<ACGBotController>(OwnerComp.GetOwner());

	if (Controller == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// Get the pawns.
	APawn* Pawn = Controller->GetPawn();
	ACGCharacter* Enemy = Controller->GetEnemy();

	if (Pawn && Enemy && Enemy->IsAlive())
	{
		// Find a point between the actors to serve as an origin.
		const FVector PointBetweenActors = Enemy->GetActorLocation() + 400.f * (Pawn->GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();

		// Find a location on the nav mesh.
		const FVector Target = UNavigationSystem::GetRandomPointInRadius(Controller, PointBetweenActors, 200.f);

		// Guard against bad locations.
		if (Target != FVector::ZeroVector)
		{
			OwnerComp.GetBlackboardComponent()->SetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID(), Target);
			return EBTNodeResult::Succeeded;
		}
	}


	return EBTNodeResult::Failed;
}

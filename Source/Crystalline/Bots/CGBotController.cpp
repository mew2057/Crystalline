// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Bots/CGBot.h"
#include "CGBotController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"




ACGBotController::ACGBotController(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	BlackboardComp = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("BlackBoardComp"));

	BehaviorComp = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorComp"));


	bWantsPlayerState = true;
}

void ACGBotController::Possess(class APawn* InPawn)
{
	Super::Possess(InPawn);

	ACGBot* Bot = Cast<ACGBot>(InPawn);


	if (Bot && Bot->BotBehavior)
	{
		// Give the Controller the Blackboard.
		BlackboardComp->InitializeBlackboard(Bot->BotBehavior->BlackboardAsset);

		// Get the keys from the blackboard.
		EnemyKeyID = BlackboardComp->GetKeyID("Enemy");
		DestinationKeyID = BlackboardComp->GetKeyID("Destination");

		// Start the behavior tree.
		BehaviorComp->StartTree(*(Bot->BotBehavior));
	}
}


void ACGBotController::BeginInactiveState()
{
	Super::BeginInactiveState();

	AGameState* GameState = GetWorld()->GameState;

	const float MinRespawnDelay = (GameState && GameState->GameModeClass) ? GetDefault<AGameMode>(GameState->GameModeClass)->MinRespawnDelay : 1.0f;

	GetWorldTimerManager().SetTimer(this, &ACGBotController::Respawn, MinRespawnDelay);
}

void ACGBotController::Respawn()
{
	GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}



void ACGBotController::SearchForEnemy()
{
	APawn* MyBot = GetPawn();

	if (MyBot == NULL)
	{
		return;
	}

	const FVector MyLoc = MyBot->GetActorLocation();
	float BestDistSq = MAX_FLT;
	float TempDist = 0;
	ACGCharacter* BestEnemy = NULL;
	ACGCharacter* TempEnemy = NULL;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		TempEnemy = Cast<ACGCharacter>(*It);
		
		// TODO improve robustness for team game types.
		if (TempEnemy && TempEnemy->Controller != this)
		{
			// TODO Check to see if DistSquared or SizeSquared is faster.
			TempDist = FVector::DistSquared(MyLoc, TempEnemy->GetActorLocation());
			if (TempDist < BestDistSq)
			{
				BestDistSq = TempDist;
				BestEnemy = TempEnemy;
			}
		}
	}

	if (BestEnemy)
	{
		SetEnemy(BestEnemy);
	}

}

void ACGBotController::SetEnemy(class APawn* InPawn)
{
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(EnemyKeyID, InPawn);
		SetFocus(InPawn);
	}
}

ACGCharacter* ACGBotController::GetEnemy()
{
	if (BlackboardComp)
	{
		return Cast<ACGCharacter>(BlackboardComp->GetValueAsObject(EnemyKeyID));
	}

	return nullptr;
}

void ACGBotController::ShootEnemy()
{

}
// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "Bots/CGBot.h"
#include "CGBotController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"



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
		BlackboardComp->InitializeBlackboard(*(Bot->BotBehavior->BlackboardAsset));

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

	GetWorldTimerManager().SetTimer(TimerHandle_Respawn, this, &ACGBotController::Respawn, MinRespawnDelay);
}

void ACGBotController::Respawn()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Respawn);
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
		if (TempEnemy && TempEnemy->Controller != this && TempEnemy->IsAlive())
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
		BlackboardComp->SetValue<UBlackboardKeyType_Object>(EnemyKeyID, InPawn);
		SetFocus(InPawn);

	}
}

ACGCharacter* ACGBotController::GetEnemy()
{
	if (BlackboardComp)
	{
		
		return Cast<ACGCharacter>(BlackboardComp->GetValue<UBlackboardKeyType_Object>(EnemyKeyID));
	}

	return nullptr;
}

void ACGBotController::ShootEnemy()
{
	// Get pawn and weapon, get enemy and LOS, shoot or not.
	ACGCharacter* MyBot = Cast<ACGCharacter>(GetPawn());

	if (MyBot == nullptr || MyBot->GetCurrentWeapon() == nullptr)
	{
		return;
	}
	
	ACGWeapon* CurrentWeapon = MyBot->GetCurrentWeapon();
	ACGCharacter* Enemy = GetEnemy();

	if (Enemy && Enemy->IsAlive() && LineOfSightTo(Enemy, MyBot->GetActorLocation()))
	{
		MyBot->StartFire();
		//CurrentWeapon->StartFiring();
	}
	else
	{
		MyBot->StopFire();

	//	CurrentWeapon->StopFiring();
	}
	
}

void ACGBotController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	const FVector Focus = GetFocalPoint();
	
	if (!Focus.IsZero() && GetPawn())
	{
		FRotator DirRotation = (Focus - GetPawn()->GetActorLocation()).Rotation();

		//Clamp so the player doesn't spin wildly.
		DirRotation.Yaw = FRotator::ClampAxis(DirRotation.Yaw);
		SetControlRotation(DirRotation);
		if (bUpdatePawn)
		{
			GetPawn()->FaceRotation(DirRotation, DeltaTime);
		}
	}
}

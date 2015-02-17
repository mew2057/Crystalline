// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCrystal.h"


ACGCrystal::ACGCrystal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	OverlapVolume = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CrystalOverlapVolume"));
	OverlapVolume->SetCapsuleHalfHeight(88.f);
	OverlapVolume->SetCapsuleRadius(80.f);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = OverlapVolume;

	CrystalMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CrystalMesh"));
	CrystalMesh->bReceivesDecals = false;
	CrystalMesh->CastShadow = true;
	CrystalMesh->bOnlyOwnerSee = false;
	CrystalMesh->bOwnerNoSee = false;
	CrystalMesh->SetCollisionObjectType(ECC_WorldStatic);
	CrystalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CrystalMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	CrystalMesh->SetHiddenInGame(false);
	CrystalMesh->RelativeLocation = FVector(0.f, 0.f, 50.f);
	CrystalMesh->AttachParent = OverlapVolume;
	
	BaseMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("CrystalBaseMesh"));
	BaseMesh->bReceivesDecals = false;                             
	BaseMesh->CastShadow = true;                                   
	BaseMesh->bOnlyOwnerSee = false;
	BaseMesh->bOwnerNoSee = false;
	BaseMesh->SetCollisionObjectType(ECC_WorldStatic);			   
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); 
	BaseMesh->SetCollisionResponseToAllChannels(ECR_Ignore);       
	BaseMesh->SetHiddenInGame(false);
	CrystalMesh->RelativeLocation = FVector(0.f, 0.f, -80.f);
	BaseMesh->AttachParent = OverlapVolume;

	// Make it so we have replication.
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;

	// TODO set is active based on spawn active.
	bIsActive = true;
	bSpawnActive = true;
	CrystalType = ECrystalType::UPGRADE;

}

void ACGCrystal::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// If not spawned as active, kick off a despawn event.
	if (!bSpawnActive)
	{
		OnDespawn();
	}	
}


void ACGCrystal::ReceiveActorBeginOverlap(class AActor* Other)
{
	// EARLY Return if the pickup is not active.
	if (!bIsActive )
	{
		return;
	}

	Super::ReceiveActorBeginOverlap(Other);
	// TODO Tell the player that they can pickup the item.
	ACGCharacter* Player = Cast<ACGCharacter>(Other);
	if (Player)
	{
		Player->OnStartCrystalOverlap(this);
	}
}

void ACGCrystal::ReceiveActorEndOverlap(class AActor* Other)
{

	Super::ReceiveActorEndOverlap(Other);

	ACGCharacter* Player = Cast<ACGCharacter>(Other);
	if (Player)
	{
		Player->OnStopCrystalOverlap(this);
	}
}

bool ACGCrystal::OnDespawn()
{
	if (!bIsActive)
	{
		return false;
	}

	// Hide the crystal.
	if (TimeToRespawn > 0)
	{
		HideCrystal();
		bIsActive = false;
		GetWorldTimerManager().SetTimer(this, &ACGCrystal::OnRespawn, TimeToRespawn, false);

		// For some reason this refused to replicate on clients 
		if (Role < ROLE_Authority)
		{
			UE_LOG(LogTemp, Log, TEXT("NOT AUTHORITY"));

			ServerOnDespawn();
		}
	}

	return true;
}

bool ACGCrystal::ServerOnDespawn_Validate()
{
	return true;
}

void ACGCrystal::ServerOnDespawn_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Server Do Despawn"));

	bIsActive = false;
}

void ACGCrystal::OnRespawn()
{
	ShowCrystal();
	bIsActive = true;

	if (Role < ROLE_Authority)
	{
		ServerOnRespawn();
	}
}

bool ACGCrystal::ServerOnRespawn_Validate()
{
	return true;
}

void ACGCrystal::ServerOnRespawn_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Server Do Respawn"));

	bIsActive = true;
}

void ACGCrystal::HideCrystal()
{
	// Disable the Crystal and Trigger Replication.
	CrystalMesh->SetHiddenInGame(true);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACGCrystal::ShowCrystal()
{
	// Enable the Show the crystal and Trigger Replication.
	bIsActive = true;
	CrystalMesh->SetHiddenInGame(false);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ACGCrystal::OnRep_Active()
{
	UE_LOG(LogTemp, Log, TEXT("ONREP"));

	if (bIsActive)
	{
		ShowCrystal();
	}
	else
	{
		HideCrystal();
	}

}

void ACGCrystal::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGCrystal, bIsActive);
}


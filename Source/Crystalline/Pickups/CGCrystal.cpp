// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCrystal.h"


ACGCrystal::ACGCrystal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	OverlapVolume = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("CrystalOverlapVolume"));
	OverlapVolume->SetCapsuleHalfHeight(88.f);
	OverlapVolume->SetCapsuleRadius(80.f);
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
	
	//TODO Stuff
	CrystalType = ECrystalType::UPGRADE;


	// Make it so we have replication.
	bReplicates = true;
	bReplicateInstigator = true;
	bNetUseOwnerRelevancy = true;
}


void ACGCrystal::ReceiveActorBeginOverlap(class AActor* Other)
{
	Super::ReceiveActorBeginOverlap(Other);
	// TODO Tell the player that they can pickup the item.
	ACGCharacter* Player = Cast<ACGCharacter>(Other);
	if (Player)
	{
		UE_LOG(LogTemp, Log, TEXT("Crystal is colliding with a player. "));
	}
	UE_LOG(LogTemp, Log, TEXT("OverLap. "));

}

void ACGCrystal::ReceiveActorEndOverlap(class AActor* Other)
{
	Super::ReceiveActorEndOverlap(Other);

	ACGCharacter* Player = Cast<ACGCharacter>(Other);
	if (Player)
	{
		UE_LOG(LogTemp, Log, TEXT("Crystal is no longer colliding with a player. "));
	}
}

bool ACGCrystal::Pickup()
{
	if (!bIsActive)
	{
		return false;
	}

	// Hide the crystal.
	HideCrystal();

	return true;
}

void ACGCrystal::HideCrystal()
{
	if (TimeToRespawn <= 0)
	{ 
		return;
	}

	GetWorldTimerManager().SetTimer(this, &ACGCrystal::ShowCrystal, TimeToRespawn, false);
}

void ACGCrystal::ShowCrystal()
{

}



void ACGCrystal::OnRep_Active()
{

}

void ACGCrystal::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGCrystal, bIsActive);
}


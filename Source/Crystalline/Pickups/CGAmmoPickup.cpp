// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGAmmoPickup.h"

ACGAmmoPickup::ACGAmmoPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Ammo = 0;
	AmmoType = ECGAmmoType::NONE;
	LifeSpan = 10.f;

	bReplicates = true;
	/*
	OverlapVolume = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("AmmoOverlapVolume"));
	OverlapVolume->SetCapsuleHalfHeight(20.f);
	OverlapVolume->SetCapsuleRadius(20.f);
	OverlapVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	//OverlapVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapVolume->SetIsReplicated(true);
	RootComponent = OverlapVolume;
	*/
	AmmoMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("AmmoMesh"));
	AmmoMesh->bReceivesDecals = false;
	AmmoMesh->CastShadow = true;
	AmmoMesh->bOnlyOwnerSee = false;
	AmmoMesh->bOwnerNoSee = false;
	AmmoMesh->SetCollisionObjectType(ECC_WorldDynamic);
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AmmoMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AmmoMesh->SetHiddenInGame(false);
	AmmoMesh->SetIsReplicated(true);
	AmmoMesh->SetSimulatePhysics(true);
	AmmoMesh->RelativeLocation = FVector(0.f, 0.f, 50.f);
	RootComponent = AmmoMesh;
}

void ACGAmmoPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}


void ACGAmmoPickup::ReceiveActorBeginOverlap(class AActor* Other)
{
	if (Role == ROLE_Authority)
	{
		ACGCharacter* Player = Cast<ACGCharacter>(Other);
		// If the player is given any ammo, kill this object.
		if (Player && Player->GiveAmmo(AmmoType,  Ammo))
		{
			ProcessDestroy();
		}
	}
}

void ACGAmmoPickup::ProcessDestroy()
{
	GetWorldTimerManager().ClearTimer(this, &ACGAmmoPickup::ProcessDestroy);
	Destroy();
}

void ACGAmmoPickup::Initialize(class AActor* Owner, int32 BaseAmmo, ECGAmmoType BaseAmmoType)
{
	Ammo = BaseAmmo;
	AmmoType = BaseAmmoType;
	AmmoMesh->IgnoreActorWhenMoving(Owner, true);
	GetWorldTimerManager().SetTimer(this, &ACGAmmoPickup::ProcessDestroy, LifeSpan, false);
}



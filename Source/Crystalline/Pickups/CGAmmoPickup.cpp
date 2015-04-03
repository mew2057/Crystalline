// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGAmmoPickup.h"

ACGAmmoPickup::ACGAmmoPickup(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Ammo = 0;
	AmmoType = ECGAmmoType::NONE;
	LifeSpan = 10.f;
	bReplicates = true;

	AmmoMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("AmmoMesh"));
	AmmoMesh->bReceivesDecals = false;
	AmmoMesh->CastShadow = true;
	AmmoMesh->bOnlyOwnerSee = false;
	AmmoMesh->bOwnerNoSee = false;
	AmmoMesh->SetCollisionObjectType(ECC_WorldDynamic);
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	AmmoMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	AmmoMesh->SetCollisionObjectType(COLLISION_AMMO_DROP);
	AmmoMesh->SetHiddenInGame(false);
	AmmoMesh->SetIsReplicated(true);
	AmmoMesh->SetSimulatePhysics(false);
	AmmoMesh->RelativeLocation = FVector(0.f, 0.f, 50.f);
	RootComponent = AmmoMesh;
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
	GetWorldTimerManager().ClearTimer(TimerHandle_Destroy);
	Destroy();
}

void ACGAmmoPickup::Initialize(class AActor* Owner, int32 BaseAmmo, ECGAmmoType BaseAmmoType)
{
	// Trace to ground, set position on the ground.
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName("PickupTrace"), true, Owner);
	TraceParams.bTraceAsyncScene = true;
	TraceParams.AddIgnoredActor(this);

	// Point Down, ~10000.f long
	const FVector StartTrace = GetActorLocation();

	FVector Gravity = FVector(0.f, 0.f, GetWorld()->GetGravityZ());
	Gravity.Normalize();

	const FVector EndTrace = StartTrace + Gravity * 10000.f;

	// XXX Change Trace.
	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingle(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	// If we hit something, move to that, since networking physics can be an issue.
	if (Hit.bBlockingHit)
	{
		Ammo = BaseAmmo;
		AmmoType = BaseAmmoType;
		AmmoMesh->IgnoreActorWhenMoving(Owner, true);
		GetWorldTimerManager().SetTimer(TimerHandle_Destroy, this, &ACGAmmoPickup::ProcessDestroy, LifeSpan, false);
	
		// Get the impact normal then add a random Y rotation to the Ammo.
		SetActorRotation((Hit.ImpactNormal * FVector::ForwardVector).Rotation());
		AddActorLocalRotation(FRotator(0.f, FMath::RandRange(0.f, 360.f),0.f), false);
		
		// Set the location to the "ground"
		SetActorLocation(Hit.ImpactPoint);		
	}
	else
	{
		ProcessDestroy();
	}	
}

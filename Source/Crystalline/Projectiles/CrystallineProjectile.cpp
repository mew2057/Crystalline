// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "Crystalline.h"
#include "CrystallineProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACrystallineProjectile::ACrystallineProjectile(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	// Use a sphere as a simple collision representation. TODO collision object type.
	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(COLLISION_PROJECTILE);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComp;

	// Use a MovementCompComponent to govern this projectile's movement
	MovementComp = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 5000.f;
	MovementComp->MaxSpeed = 5000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bInitialVelocityInLocalSpace = false; // If this isn't set there isn't a guarantee on certain assumptions employed by Weapons.

	// Allows for a tick to be registered.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateInstigator = true;
	bReplicateMovement = true;
}


void ACrystallineProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MovementComp->OnProjectileStop.AddDynamic(this, &ACrystallineProjectile::OnImpact);
	CollisionComp->MoveIgnoreActors.Add(Instigator);
	
	SetLifeSpan(3.f);
}


void ACrystallineProjectile::OnImpact(const FHitResult& Hit)
{
	if (Role == ROLE_Authority)
	{
		Destroy();
		MovementComp->StopMovementImmediately();
	}
	// Only add impulse and destroy projectile if we hit a physics
	/*if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		
	}*/
}

void ACrystallineProjectile::SetVelocity(FVector Direction)
{
	if (MovementComp)
	{
		MovementComp->Velocity = Direction * MovementComp->InitialSpeed;
	}
}

void ACrystallineProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MovementComp)
	{
		MovementComp->Velocity = NewVelocity;
	}
}
/*
void ACrystallineProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}*/
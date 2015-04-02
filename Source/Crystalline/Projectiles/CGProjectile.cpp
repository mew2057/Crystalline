// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


ACGProjectile::ACGProjectile(const FObjectInitializer& ObjectInitializer)
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
	CollisionComp->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Ignore);
	
	RootComponent = CollisionComp;

	// Use a MovementCompComponent to govern this projectile's movement
	MovementComp = ObjectInitializer.CreateDefaultSubobject<UProjectileMovementComponent>(this, TEXT("ProjectileComp"));
	MovementComp->UpdatedComponent = CollisionComp;
	MovementComp->InitialSpeed = 5000.f;
	MovementComp->MaxSpeed = 5000.f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->ProjectileGravityScale = 0.f;
	MovementComp->bInitialVelocityInLocalSpace = false; // If this isn't set there isn't a guarantee on certain assumptions employed by Weapons.
	MovementComp->OnProjectileStop.AddDynamic(this, &ACGProjectile::OnStop);

	// Allows for a tick to be registered.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bReplicateMovement = true;
}


void ACGProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Ignore the weapon that fired the weapon.
	CollisionComp->MoveIgnoreActors.Add(Instigator);

	// Spawn the trail particles.
	SpawnTrailParticleSystem();
}

void ACGProjectile::OnStop(const FHitResult& Hit)
{
	// Only the remote is allowed to play the impact.
	if (Role == ROLE_Authority)
	{
		ProcessImpact(Hit);
		PrepForDestroy();
	}

	// XXX Doesn't play the effect when the weapon destroys on server.
	// Plays the impact regardless of ownership.
	SpawnImpact();
}

void ACGProjectile::ProcessImpact(const FHitResult& Hit)
{
	AActor* OtherActor = Hit.GetActor();

	if (OtherActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Other Actor Found: %s"), *OtherActor->GetName());

		FPointDamageEvent PointDmg;
		PointDmg.DamageTypeClass = DamageType;
		PointDmg.HitInfo = Hit;
		PointDmg.ShotDirection = Hit.ImpactNormal;
		PointDmg.Damage = ImpactDamage; // This needs to move.

		OtherActor->TakeDamage(PointDmg.Damage, PointDmg, GetInstigatorController(), this);
	}

	/*
	else
	{
	UPrimitiveComponent * OtherComponent = Hit.GetComponent();
	if (OtherComponent)
	{
	UE_LOG(LogTemp, Warning, TEXT("Other Component Found: %s"), *OtherComponent->GetName());

	OtherComponent->AddImpulseAtLocation(GetVelocity() * 100.0f, Hit.ImpactPoint);
	}
	}*/
}

void ACGProjectile::SpawnImpact()
{
	// TOOD Make this more than garbage.
	if (ImpactEffect)
	{
		UParticleSystemComponent* ImpactPSC = UGameplayStatics::SpawnEmitterAtLocation(
			this,
			ImpactEffect,
			GetActorLocation(),
			GetActorRotation());
	}
}

void ACGProjectile::PrepForDestroy()
{
	MovementComp->StopMovementImmediately();
	Destroy();
}

void ACGProjectile::SpawnTrailParticleSystem()
{
	// TODO Move this to sub classes as needed!
	if (ProjectileTrail)
	{
		TrailPSC = UGameplayStatics::SpawnEmitterAttached(ProjectileTrail, RootComponent);
	}

}

void ACGProjectile::SetVelocity(FVector Direction)
{
	if (MovementComp)
	{
		MovementComp->Velocity = Direction * MovementComp->InitialSpeed;
	}
}

void ACGProjectile::PostNetReceiveVelocity(const FVector& NewVelocity)
{
	if (MovementComp)
	{
		MovementComp->Velocity = NewVelocity;
	}
}

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
	//MovementComp->OnProjectileStop.AddDynamic(this, &ACGProjectile::OnImpact);
	CollisionComp->OnComponentHit.AddDynamic(this, &ACGProjectile::OnHit);
	CollisionComp->MoveIgnoreActors.Add(Instigator);

	SpawnTrailParticleSystem();
}

void ACGProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ECollisionResponse temp = OtherComp->GetCollisionResponseToChannel(CollisionComp->GetCollisionObjectType());
	
	if (Role == ROLE_Authority)
	{
		ProcessImpact(Hit);
		PrepForDestroy();
	}

	SpawnImpact();

	/*
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}*/
}

void ACGProjectile::OnImpact(const FHitResult& Hit)
{
	if (Role == ROLE_Authority)
	{
		ProcessImpact(Hit);
		PrepForDestroy();
	}
}

void ACGProjectile::ProcessImpact(const FHitResult& Hit)
{
	if (Hit.GetActor())
	{
		FPointDamageEvent PointDmg;
		PointDmg.DamageTypeClass = DamageType;
		PointDmg.HitInfo = Hit;
		PointDmg.ShotDirection = Hit.ImpactNormal;
		PointDmg.Damage = ImpactDamage; // This needs to move.

		Hit.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, GetInstigatorController(), this);
	}

	// TODO  Add Impulse!
	// Only add impulse and destroy projectile if we hit a physics
	/*if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());


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



void ACGProjectile::OnRep_Impacted()
{
	//////////////////////////////////////////////////////////
	// TODO make sure this works on sub instances (borrowed from the unreal example).
	FVector ProjDirection = GetActorRotation().Vector();

	const FVector StartTrace = GetActorLocation() - ProjDirection * 100;
	const FVector EndTrace = GetActorLocation() + ProjDirection * 100;
	FHitResult Impact;

	// This ensures we get the surface data (assuming the explosion worked).
	if (!GetWorld()->LineTraceSingle(Impact, StartTrace, EndTrace, COLLISION_PROJECTILE, FCollisionQueryParams(TEXT("ProjectileClient"), true, Instigator)))
	{
		// This is missing surface data which is important for things having sounds matching a particular surface tpe.
		Impact.ImpactPoint = GetActorLocation();
		Impact.ImpactNormal = -ProjDirection;
	}

	//////////////////////////////////////////////////////////

	ProcessImpact(Impact);
}

void ACGProjectile::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicates to all.
	DOREPLIFETIME(ACGProjectile, bImpacted);
}


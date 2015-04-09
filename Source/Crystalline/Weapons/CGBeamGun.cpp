// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBeamGun.h"

ACGBeamGun::ACGBeamGun(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.AmmoType = ECGAmmoType::T_ONE;
}

void ACGBeamGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	MaxAngle = FMath::Cos(FMath::DegreesToRadians(MaxAngle));
}


void ACGBeamGun::FireHitScan()
{
	//Super::FireHitScan();
	
	// Perform a raycast from the crosshair in to the world space.
	// Get the starting location and rotation for the player.
	const FVector StartTrace = GetCameraLocation();
	FVector ShootDir = GetCameraAim();
	
	// Specify the end point for the weapon's fire.
	FVector EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;

	// Get the Impact for the weapon trace then confirm whether or not it hit a player.
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	
	/*
	ACGCharacter* TempTarget = Cast<ACGCharacter>(Impact.GetActor());

	// If a new pawn has come "closer" change our target.
	if (TempTarget && TempTarget->IsAlive())
	{
		Target = TempTarget;
	}

	// TODO move this to the server.
	if (Target)
	{
		FVector TargetDir = Target->GetActorLocation() - StartTrace;
		ShootDir = ShootDir.CosineAngle2D(TargetDir) >= MaxAngle ? TargetDir : ShootDir;
		EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;

		Impact = WeaponTrace(StartTrace, EndTrace);
	}	
	
	ProcessHitScan(Impact, StartTrace, ShootDir, 0, CurrentSpread);
	*/

	// If we're the client notify the Beam that we've fired the beam gun.
	if (CGOwner && CGOwner->IsLocallyControlled() && GetNetMode() == NM_Client)
	{
		ServerNotifyBeamFire(Impact, ShootDir);
	}

	// Process the beam and play any beam effects.
	ProcessBeam(Impact, ShootDir);
}

bool ACGBeamGun::ServerNotifyBeamFire_Validate(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir)
{
	return true;
}

void ACGBeamGun::ServerNotifyBeamFire_Implementation(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir)
{
	if (Instigator)
	{
		// TODO Cheat Checking
		ProcessBeam(Impact, ShootDir);
	}
}


void ACGBeamGun::ProcessBeam(const FHitResult& Impact, FVector_NetQuantizeNormal ShootDir)
{
	AActor* TempTarget = Impact.GetActor();

	// Flag to prevent an unecessary trace.
	//bool bTargetHitInImpact = Cast<APawn>(TempTarget);

	// If the sighted target is alive, assume they're a better candidate than whoever we're attached to.
	if (TempTarget != Target && TempTarget != NULL)
	{
		Target = TempTarget;
	}

	FHitResult NewImpact = Impact;
	FVector TargetDir = ShootDir;
	const FVector StartTrace = GetCameraLocation();

	// If the target wasn't found and we have a target, check to see if we're within the radius.
	if (TempTarget == NULL && Target != NULL)
	{		
		TargetDir = Target->GetActorLocation() - StartTrace;
		TargetDir = ShootDir.CosineAngle2D(TargetDir) >= MaxAngle ? TargetDir : ShootDir;

		FVector EndTrace = StartTrace + TargetDir * WeaponConfig.WeaponRange;

		NewImpact = WeaponTrace(StartTrace, EndTrace);

		Target = NewImpact.GetActor();
	}

	// TODO Damage based on range.
	// TODO "Sticky" Damage points, e.g. adjust position of hit to where the beam hits.
	// TODO Blow back damage.
	// TODO Adjusted noise.
	// TODO Fix weapon FX stay alive. (Maybe a timer?)

	if (ShouldDealDamage_Instant(Target))
	{
		// TODO add a distance modifier to this damage.
		DealDamage_Instant(NewImpact, TargetDir);
	}

	// This will trigger an OnRep that will prop to remote clients
	if (Role == ROLE_Authority)
	{
		HitNotify.Origin = StartTrace;
		HitNotify.Direction = TargetDir;
	}

	// Plays the local FX.
	if (GetNetMode() != NM_DedicatedServer)
	{
		FVector EndPoint = NewImpact.bBlockingHit ? NewImpact.ImpactPoint : StartTrace + TargetDir * WeaponConfig.WeaponRange;
		// Do spawning here.
		SpawnTrailEffect(EndPoint);
		SpawnHitEffect(NewImpact);
	}
}

void ACGBeamGun::StopWeaponFireSimulation()
{
	Super::StopWeaponFireSimulation();

	if (TrailPSC)
	{
		TrailPSC->DeactivateSystem();
		TrailPSC = NULL;
	}
}

void ACGBeamGun::SpawnTrailEffect(const FVector& EndPoint)
{
	if (WeaponFXConfig.WeaponTrail && TrailPSC == NULL)
	{
		USkeletalMeshComponent* Mesh = GetWeaponMesh();

		TrailPSC = UGameplayStatics::SpawnEmitterAttached(WeaponFXConfig.WeaponTrail, Mesh, WeaponFXConfig.MuzzleSocket);

		if (TrailPSC)
		{
			// Set the vector for the particle.
			TrailPSC->SetVectorParameter(WeaponFXConfig.TrailTargetParam, EndPoint);
		}
	}
	else if (TrailPSC)
	{

		TrailPSC->SetVectorParameter(WeaponFXConfig.TrailTargetParam, EndPoint);
	}
}

void ACGBeamGun::StopFire()
{
	Super::StopFire();
	Target = NULL;
}



void ACGBeamGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGBeamGun, Target, COND_OwnerOnly);

}

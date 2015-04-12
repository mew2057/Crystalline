// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "ParticleEmitterInstances.h"
#include "CGBeamGun.h"


ACGBeamGun::ACGBeamGun(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.AmmoType = ECGAmmoType::T_ONE;
}

void ACGBeamGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	MaxAngle = 1 - FMath::Cos(FMath::DegreesToRadians(MaxAngle));
	LockStrength = 2.f;
}


void ACGBeamGun::FireHitScan()
{
	// Perform a raycast from the crosshair in to the world space.
	// Get the starting location and rotation for the player.
	const FVector StartTrace = GetCameraLocation();
	FVector ShootDir = GetCameraAim();
	
	// Specify the end point for the weapon's fire.
	FVector EndTrace = StartTrace + ShootDir * WeaponConfig.WeaponRange;

	// Get the Impact for the weapon trace then confirm whether or not it hit a player.
	FHitResult Impact = WeaponTrace(StartTrace, EndTrace);

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
	
	// If the sighted target is alive, assume they're a better candidate than whoever we're attached to.
	if (TempTarget != NULL )
	{
		if (TempTarget->bCanBeDamaged)
		{
			if (TempTarget != Target)
			{
				Target = TempTarget;
			}

			TargetLocation = Impact.ImpactPoint - Target->GetActorLocation();
		}
		else
		{
			TempTarget = NULL;
		}
	}

	FHitResult NewImpact = Impact;
	FVector TargetDir = ShootDir;
	const FVector StartTrace = GetCameraLocation();
	float Angle = 0.f;

	// If the target wasn't found and we have a target, check to see if we're within the radius.
	if (TempTarget == NULL && Target != NULL)
	{		
		TargetDir = (TargetLocation + Target->GetActorLocation()) - StartTrace;
		TargetDir.Normalize();
		Angle = 1 - FVector::DotProduct(TargetDir, ShootDir);

		TargetDir = Angle <= MaxAngle ? TargetDir : ShootDir;

		FVector EndTrace = StartTrace + TargetDir * WeaponConfig.WeaponRange;

		NewImpact = WeaponTrace(StartTrace, EndTrace);

		Target = NewImpact.GetActor();
	}
	

	// TODO Damage based on range.
	// TODO Blow back damage.

	LockStrength = Target != NULL ? Angle / MaxAngle : 1.f;


	if (ShouldDealDamage_Instant(Target))
	{		
		// Perform the Dist check for computing the damage.
		const float DistToTarget = FVector::Dist((TargetLocation + Target->GetActorLocation()), StartTrace);

		// TODO add a distance modifier to this damage.
		DealDamageBeam(NewImpact, TargetDir, DistToTarget);
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

void ACGBeamGun::DealDamageBeam(const FHitResult& Impact, const FVector& ShootDir, float Dist)
{
	// The value of the damage curve at this distance.
	//const float CurveValue = DamageCurve != NULL ? DamageCurve->GetValue(Dist / WeaponConfig.WeaponRange) : 1.f;

	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = WeaponConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = WeaponConfig.BaseDamage;

	if (PointDmg.Damage > 0.f)
	{
		Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, CGOwner->Controller, this);
	}
	else if (PointDmg.Damage < 0.f)
	{
		CGOwner->TakeDamage(-PointDmg.Damage, PointDmg, CGOwner->Controller, this);
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
	// EARLY RETURN If the burst count is zero or less, the counter got mixed up.
	if (BurstCount <= 0)
	{
		return;
	}

	if (WeaponFXConfig.WeaponTrail)
	{
		USkeletalMeshComponent* Mesh = GetWeaponMesh();
		TrailPSC = UGameplayStatics::SpawnEmitterAttached(LockStrength < 1.f ? ConnectedBeam : WeaponFXConfig.WeaponTrail, Mesh, WeaponFXConfig.MuzzleSocket);

		if (TrailPSC)
		{
			// Set the vector for the particle.
			TrailPSC->SetVectorParameter(WeaponFXConfig.TrailTargetParam, EndPoint);
			
			FVector NewColor = FMath::Lerp(HitColor, NoHitColor, LockStrength);
			TrailPSC->SetVectorParameter(BEAM_COLOR_OVER_LIFE, NewColor);	
		}
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

	DOREPLIFETIME(ACGBeamGun, LockStrength);
}

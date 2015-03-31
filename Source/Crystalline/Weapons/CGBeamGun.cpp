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
}


void ACGBeamGun::ProcessHitScanConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread)
{
	//Target = Cast<APawn>(Impact.GetActor());

	Super::ProcessHitScanConfirmed(Impact, Origin, ShootDir, RandSeed, Spread);	
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


/*
void ACGBeamGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ACGBeamGun, Target, COND_OwnerOnly);

}
*/
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
	// If there's a target, then the gun should maintain its connection if the connection is broken check for a target.
	if (Target)
	{
		const FVector AimDir = GetCameraAim();
		const FVector Origin = GetCameraLocation();
		FVector TargetDir = Target->GetActorLocation() - Origin;
		const float Offset = AimDir.CosineAngle2D(TargetDir);


		if (Offset >= MaxAngle)
		{
			// This might be paranoia.
			TargetDir.Normalize();
			const FVector EndTrace = TargetDir * WeaponConfig.WeaponRange;
			FHitResult Impact = WeaponTrace(Origin, EndTrace);
			// This may be a point for error.
			ProcessHitScan(Impact, Origin, TargetDir, 0, CurrentSpread);
		}
		else
		{
			// Null the target, because we aren't tracking them anymore.
			Target = NULL;

			// Break the connection
			Super::FireHitScan();
		}
	}
	else
	{
		Super::FireHitScan();
	}
}


void ACGBeamGun::ProcessHitScanConfirmed(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir, int32 RandSeed, float Spread)
{
	Target = Impact.GetActor();

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
		TrailPSC = UGameplayStatics::SpawnEmitterAttached(WeaponFXConfig.WeaponTrail, Mesh1P, WeaponFXConfig.MuzzleSocket);

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

	DOREPLIFETIME(ACGBeamGun, Target);

}

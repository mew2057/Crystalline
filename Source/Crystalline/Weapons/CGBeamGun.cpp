// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBeamGun.h"

void ACGBeamGun::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	MaxAngle = FMath::Cos(MaxAngle);
}


void ACGBeamGun::FireHitScan()
{
	// If there's a target, we're doing okay.
	if (Target)
	{
		const FVector AimDir = GetCameraAim();
		const FVector TargetDir = Target->GetActorLocation() - GetCameraLocation();
		const float Offset = AimDir.CosineAngle2D(TargetDir);

		if (Offset > MaxAngle)
		{
			// Break the connection
		}
		else
		{
			// Deal Damage (if any)
		}
	}


	// Perform a typical hitscan.
}




void ACGBeamGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGBeamGun, Target);

}

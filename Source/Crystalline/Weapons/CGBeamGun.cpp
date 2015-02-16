// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGBeamGun.h"




void ACGBeamGun::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACGBeamGun, Target);

}

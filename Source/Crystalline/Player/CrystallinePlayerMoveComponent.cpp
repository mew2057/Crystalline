// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePlayerMoveComponent.h"

float UCrystallinePlayerMoveComponent::GetMaxSpeed() const
{
	float MaxSpeed = Super::GetMaxSpeed();
	
	const ACrystallinePlayer* PlayerOwner = Cast<ACrystallinePlayer>(PawnOwner);
	if (PlayerOwner)
	{
		if (PlayerOwner->IsRunning())
		{
			MaxSpeed *= PlayerOwner->GetRunningSpeedModifier();
		}
	}
	
	return MaxSpeed;

}
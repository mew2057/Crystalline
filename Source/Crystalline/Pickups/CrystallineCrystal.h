// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CrystallineCrystal.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACrystallineCrystal : public AActor
{
	GENERATED_UCLASS_BODY()

	/** Triggers the pickup prompt for the crystal, if the crystal gun is equipped.*/
	virtual void ACrystallineCrystal::ReceiveActorBeginOverlap(class AActor* Other) override;
};

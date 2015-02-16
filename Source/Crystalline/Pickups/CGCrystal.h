// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGCrystal.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGCrystal : public AActor
{
	GENERATED_BODY()
public:
	ACGCrystal(const FObjectInitializer& ObjectInitializer);

	/** Triggers the pickup prompt for the crystal, if the crystal gun is equipped.*/
	virtual void ACGCrystal::ReceiveActorBeginOverlap(class AActor* Other) override;
};

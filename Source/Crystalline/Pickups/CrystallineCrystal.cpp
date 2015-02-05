// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallineCrystal.h"


ACrystallineCrystal::ACrystallineCrystal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//TODO Stuff
}


void ACrystallineCrystal::ReceiveActorBeginOverlap(class AActor* Other) 
{
	Super::ReceiveActorBeginOverlap(Other);
	// TODO Tell the player that they can pickup the item.
}


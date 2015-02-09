// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGCrystal.h"


ACGCrystal::ACGCrystal(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	//TODO Stuff
}


void ACGCrystal::ReceiveActorBeginOverlap(class AActor* Other)
{
	Super::ReceiveActorBeginOverlap(Other);
	// TODO Tell the player that they can pickup the item.
}


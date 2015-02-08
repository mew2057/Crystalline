// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponActiveState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponActiveState : public UCGWeaponState
{
	GENERATED_BODY()

public:
	UCGWeaponActiveState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}
	
	
};

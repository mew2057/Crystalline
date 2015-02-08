// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponReloadingState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponReloadingState : public UCGWeaponState
{
	GENERATED_BODY()
	
public:	
	UCGWeaponReloadingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}
	
	
};

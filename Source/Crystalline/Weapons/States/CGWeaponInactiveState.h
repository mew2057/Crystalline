// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponInactiveState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponInactiveState : public UCGWeaponState
{
	GENERATED_BODY()

public:
	UCGWeaponInactiveState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}
	

	virtual bool StartEquip() override
	{
		GetOuterACGWeapon()->GotoEquippingState();
		return true;
	}
	virtual void StartReload() final { }

};

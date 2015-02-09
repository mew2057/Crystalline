// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponEquippingState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class UCGWeaponEquippingState : public UCGWeaponState
{
	GENERATED_BODY()

public:
	UCGWeaponEquippingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{}

	virtual void EnterState() override
	{
		GetCGOwner()->GetWorldTimerManager().SetTimer(this, &UCGWeaponEquippingState::EquipFinished, GetOuterACGWeapon()->WeaponConfig.EquipTime);
		UE_LOG(LogTemp, Log, TEXT("Starting Equip State"));

	}

	virtual void StartEquip() override
	{
		// TODO React to an equip in the middle of equipping.
	}

	virtual void EquipFinished() 
	{
		GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->ActiveState);
		UE_LOG(LogTemp, Log, TEXT("Going Active"));

	}

	virtual void StartFire() override {}
	

	virtual void EndState() override
	{
		GetCGOwner()->GetWorldTimerManager().ClearTimer(this, &UCGWeaponEquippingState::EquipFinished);
	}

};

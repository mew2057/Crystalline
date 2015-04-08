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
protected:
	/**Timer Handle for the Shield Regeneration timer.*/
	FTimerHandle TimerHandle_Equipping;

public:
	UCGWeaponEquippingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	virtual void EnterState() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Entering Equipping state %s"), GetCGOwner()->Role == ROLE_Authority ? TEXT("SERVER") : TEXT("CLIENT"));

		GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Equipping, this, &UCGWeaponEquippingState::EquipFinished, GetOuterACGWeapon()->WeaponConfig.EquipTime, false);
	}

	virtual bool StartEquip() override
	{
		// TODO React to an equip in the middle of equipping.
		return false;
	}

	virtual void EquipFinished() 
	{
		GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->ActiveState);
	}
	
	virtual void StartReload() final { }

	virtual void EndState() override
	{
		UE_LOG(LogTemp, Warning, TEXT("Exiting Equipping state %s"), GetCGOwner()->Role == ROLE_Authority ? TEXT("SERVER") : TEXT("CLIENT"));

		if (GetCGOwner())
		{
			GetCGOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Equipping);
		}
	}

};

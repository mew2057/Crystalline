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
		// play this sound only locally.
		if (GetCGOwner() && GetCGOwner()->IsLocallyControlled())
		{
			GetOuterACGWeapon()->PlayWeaponSound(GetOuterACGWeapon()->EquipSound);
		}

		// XXX This might be better to use the animation time 
		// Determine the length of the animation.
		float EquipTime = GetOuterACGWeapon()->PlayWeaponAnimation(GetOuterACGWeapon()->EquipAnim);
		UE_LOG(LogTemp, Warning, TEXT("Equip Time %f"), EquipTime);
		

		if (EquipTime <= 0.f)
		{
			EquipTime = GetOuterACGWeapon()->WeaponConfig.EquipTime;
		}

		GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Equipping, this, &UCGWeaponEquippingState::EquipFinished, EquipTime, false);
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
	//	GetOuterACGWeapon()->StopWeaponAnimation(GetOuterACGWeapon()->EquipAnim);

		if (GetCGOwner())
		{
			GetCGOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Equipping);
		}
	}

};

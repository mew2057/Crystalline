// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/States/CGWeaponState.h"
#include "CGWeaponUnequippingState.generated.h"

/**
 * 
 */
UCLASS(CustomConstructor)
class  UCGWeaponUnequippingState : public UCGWeaponState
{
	GENERATED_BODY()

protected:

	/**Timer Handle for the Unequip timer.*/
	FTimerHandle TimerHandle_Unequip;
public:

	UCGWeaponUnequippingState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
	{
	}

	virtual void EnterState() override
	{
		// play this sound only locally.
		if (GetCGOwner() && GetCGOwner()->IsLocallyControlled())
		{
			GetOuterACGWeapon()->PlayWeaponSound(GetOuterACGWeapon()->UnequipSound);
		}
		
		// XXX This might be better to use the animation time 
		// Determine the length of the animation.
		float UnequipTime = GetOuterACGWeapon()->PlayWeaponAnimation(GetOuterACGWeapon()->UnequipAnim);
		UE_LOG(LogTemp, Warning, TEXT("UnEquip Time %f"), UnequipTime);

		if (UnequipTime <= 0.f)
		{
			UnequipTime = GetOuterACGWeapon()->WeaponConfig.UnequipTime;
		}

		GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Unequip, this, &UCGWeaponUnequippingState::UnequipFinished, UnequipTime, false);
	}
	
	void UnequipFinished()
	{
		// XXX May need to track time in here at some point, but I'm going to wait for Matt's anims, because that's polish. -John
		GetOuterACGWeapon()->DetachMeshFromPawn();
		GetOuterACGWeapon()->GotoState(GetOuterACGWeapon()->InactiveState);
		GetCGOwner()->WeaponChanged();
	}

	virtual void EndState() override
	{
		GetOuterACGWeapon()->StopWeaponAnimation(GetOuterACGWeapon()->UnequipAnim);
		GetCGOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Unequip);
	}
};

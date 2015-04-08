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
		UE_LOG(LogTemp, Warning, TEXT("Entering Unequipping state %s"), GetCGOwner()->Role == ROLE_Authority ? TEXT("SERVER") : TEXT("CLIENT"));

		GetCGOwner()->GetWorldTimerManager().SetTimer(TimerHandle_Unequip, this, &UCGWeaponUnequippingState::UnequipFinished, GetOuterACGWeapon()->WeaponConfig.UnequipTime);
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
		UE_LOG(LogTemp, Warning, TEXT("Exiting Unequipping state %s"), GetCGOwner()->Role == ROLE_Authority ? TEXT("SERVER") : TEXT("CLIENT"));

		GetCGOwner()->GetWorldTimerManager().ClearTimer(TimerHandle_Unequip);
	}
};

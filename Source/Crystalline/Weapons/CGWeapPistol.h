// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGWeapon.h"
#include "CGWeapPistol.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class CRYSTALLINE_API ACGWeapPistol : public ACGWeapon
{
	GENERATED_BODY()
public:
	ACGWeapPistol(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaSeconds) override;

	//virtual void OnStartReload() override{}

#pragma region Ammo

	virtual void UseAmmo() override;

	virtual bool CanFire(bool InitFireCheck = false) const override;

	virtual float GetClipPercent() const override;

	virtual bool ShouldDisplayAmmoWarning() const override { return bIsOverheated || CurrentState == ReloadingState; };

	virtual bool CanReload() const override;

	virtual void StartOverheat() override;

	void BeginCooldown();

	virtual int32 GetAmmoInClip() const  override{ return WeaponHeat; }


#pragma endregion

	
protected:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGOverheatAmmoData OverheatConfig;

	/** Identifies if the pistol is in the overheated state, no shots are allowed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	uint32 bIsOverheated : 1;

	/** Identifies if the pistol is in the overheated state, no shots are allowed. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	uint32 bIsCoolingDown : 1;

	////////////////////////////////////
	// Overheat Mechanic
	/** The current heat of the weapon, Zero at start.*/
	UPROPERTY(Transient, Replicated)
	float WeaponHeat;

	/**Timer Handle for the Overheat timer.*/
	FTimerHandle TimerHandle_Overheat;


};

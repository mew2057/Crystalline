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

#pragma region Ammo

	virtual void UseAmmo() override;

	virtual bool CanFire() const override;

	virtual float GetClipPercent() const override;

	virtual void StartCooldown() override;

	virtual void EndCooldown() override;

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
	UPROPERTY(Transient)
	float WeaponHeat;



};

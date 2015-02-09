// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGWeapon.h"
#include "CGCrystalGun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGCrystalGun : public ACGWeapon
{
	GENERATED_BODY()
	
public:
	ACGCrystalGun(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;


#pragma region Ammo

	virtual void UseAmmo() override;

	virtual bool CanFire() const override;

	virtual float GetClipPercent() const override;

	virtual float GetReloadTime() const override;

	virtual bool GetCanReload() const override;

	virtual void ApplyReload() override;

	virtual int32 GetAmmo() const override { return Ammo; }

	virtual int32 GetAmmoInClip() const override { return AmmoInClip; }

#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGAmmoData AmmoConfig;

	/**This is the ammo in addition to the ammo in the clip.*/
	UPROPERTY(Transient, Replicated)
	float Ammo;
	
	UPROPERTY(Transient, Replicated)
	float AmmoInClip;
	
	
};

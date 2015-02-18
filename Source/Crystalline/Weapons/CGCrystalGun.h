// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGWeapon.h"
#include "Weapons/CGAmmo.h"
#include "CGCrystalGun.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGCrystalGun : public ACGWeapon
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGAmmoData AmmoConfig;

	UPROPERTY(VisibleAnywhere, Replicated)
	class UCGAmmo* CrystalAmmo;

public:
	ACGCrystalGun(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;


#pragma region Ammo

	virtual void UseAmmo() override;

	virtual bool CanFire(bool InitFireCheck) const override;

	virtual float GetClipPercent() const override;

	virtual float GetReloadTime() const override;

	virtual bool GetCanReload() const override;

	virtual void ApplyReload() override;

	virtual int32 GetAmmo() const override { return CrystalAmmo->AmmoCarried; }

	virtual int32 GetAmmoInClip() const override { return CrystalAmmo->AmmoInClip; }

	FORCEINLINE void SetWeaponAmmo( UCGAmmo* const Ammo) { CrystalAmmo = Ammo; }
#pragma endregion


	
};

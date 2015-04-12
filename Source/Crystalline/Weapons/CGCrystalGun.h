// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Weapons/CGWeapon.h"
#include "CGCrystalGun.generated.h"

#define CG_PERCENT_FLASH .1f
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
	
	virtual void GiveAmmo(int32 NewAmmo) override;

	virtual void UseAmmo() override;

	virtual bool CanFire(bool InitFireCheck) const override;

	virtual float GetClipPercent() const override;

	virtual float GetShotsPerClip() const override;

	virtual float GetReloadTime() const override;

	virtual bool ShouldDisplayAmmoWarning() const override { return AmmoInClip <= FlashLevel; };

	virtual bool CanReload() const override;

	virtual void ApplyReload() override;

	// XXX Maybe make the ammo convert wholesale?
	virtual int32 GetAmmo() const override { return Ammo / AmmoConfig.AmmoPerShot; }
	FORCEINLINE int32 GetActualAmmo() const { return Ammo; }


	virtual int32 GetDroppableAmmo() const override { return Ammo + AmmoInClip; }

	// XXX Maybe make the ammo convert wholesale?
	virtual int32 GetAmmoInClip() const override { return AmmoInClip / AmmoConfig.AmmoPerShot; }
	FORCEINLINE int32 GetActualAmmoInClip() const { return AmmoInClip; }


	void InitializeAmmo(const FCGCrystalAmmo& AmmoStruct);

	void CopyAmmo(int32 Ammo, int32 AmmoInClip);
#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FCGAmmoData AmmoConfig;

	/**This is the ammo in addition to the ammo in the clip.*/
	UPROPERTY(Transient, Replicated)
	int32 Ammo;
	
	UPROPERTY(Transient, Replicated)
	int32 AmmoInClip; 

	/** Ammo Count at which the gun should flash.*/
	int32 FlashLevel;
};

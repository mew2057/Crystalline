// Fill out your copyright notice in the Description page of Project Settings.

#include "CGTypes.generated.h"

#pragma once

#pragma region Enums

UENUM(BlueprintType)
enum class ECGAmmoType : uint8
{
	NONE UMETA(DisplayName = "None"),
	T_ZERO UMETA(DisplayName = "Tier Zero"),
	T_ONE  UMETA(DisplayName = "Tier One"),
	T_TWO  UMETA(DisplayName = "Tier Two")
};

UENUM(BlueprintType)
enum class ECGCrystalType : uint8
{
	NONE UMETA(DisplayName = "None"),
	POWER_UP UMETA(DisplayName = "Power Up"),
	FORCE UMETA(DisplayName = "Force"),
	ACCURACY  UMETA(DisplayName = "Accuracy"),
	UTILITY	 UMETA(DisplayName = "Utility")
};
/** Implemented for TMap.*/
inline uint8 GetTypeHash(const ECGCrystalType A)
{
	return (uint8)A;
}

#pragma endregion


#pragma region Character Structs

USTRUCT()
struct FCGZoom
{
	GENERATED_USTRUCT_BODY()
		/** The factor that this zoom.*/
		UPROPERTY(EditDefaultsOnly)
		float ZoomFactor;

	/**Speed of the interpolation from zoomed to zoomed out and vice versa. TODO make this seconds!*/
	UPROPERTY(EditDefaultsOnly)
		float ZoomSpeed;

	// Used in the zoom operation.
	///////////////////////////////////////////
	UPROPERTY(Transient)
		float TargetZoom;
	///////////////////////////////////////////

	FCGZoom()
	{
		ZoomFactor = 1.5f;
		ZoomSpeed = 20.f;
	}

public:
	/** Preps the variables of the struct for use. */
	void InitZoom()
	{
		ZoomFactor = 1 / ZoomFactor;
	}

	/** Sets the target zoom and start zoom up properly. Reduces the number of if statements per.*/
	void BeginZoom(float BaseFOV, bool bZoomIn)
	{
		TargetZoom = bZoomIn ? BaseFOV * ZoomFactor : BaseFOV;
	}

};

// TODO move this somewhere sane!
#pragma region WeaponConfigStructs


USTRUCT()
struct FCGCrystalAmmo
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly, Category = Ammo)
		int32 MaxAmmoCarried;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 AmmoCarried;

	UPROPERTY(EditAnywhere, Category = Ammo)
		int32 AmmoInClip;


	FCGCrystalAmmo()
	{
		MaxAmmoCarried = 120;
		AmmoCarried = 80;
		AmmoInClip = 0;
	}

};


USTRUCT()
struct FCGDDefaultCrystalTreeConfig
{
	GENERATED_USTRUCT_BODY()

		/** The Tier One Gun, Available when the player has a tier one crystal.*/
		UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class ACGCrystalGun> TierOneGun;

	/** The Tier Two Gun, Available when the player has a tier one and two crystal.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class ACGCrystalGun> TierTwoGun;

	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		ECGCrystalType TierOneCrystalType;
};

USTRUCT()
struct FCGDefaultWeaponConfig
{
	GENERATED_USTRUCT_BODY()

		/** An ammo free side arm.*/
		UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class ACGWeapon> CoreWeapon;

	/** The baseline crystal gun, this is always present for the player.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TSubclassOf<class ACGCrystalGun> CoreCrystalGun;

	/** Defines the Tiers for the Crystal Guns.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		TArray<FCGDDefaultCrystalTreeConfig> CrystalGunGroups;

	/**Defines the max ammo for the assault rife.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		FCGCrystalAmmo TierZeroAmmoConfig;

	/**Defines the max ammo for the tier one weapons.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		FCGCrystalAmmo TierOneAmmoConfig;

	/**Defines the max ammo for the tier two weapons.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
		FCGCrystalAmmo TierTwoAmmoConfig;

	FCGDefaultWeaponConfig()
	{
	}
};

#pragma endregion

#pragma endregion


#pragma region Weapon Structs
USTRUCT()
struct FCGWeaponConfig
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Timing)
		uint32 bAutomaticFire : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		uint32 bUsesProjectile : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		uint32 bHasManualReload : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		uint32 OverHeatWeapon : 1;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
		float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
		float EquipTime;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
		float UnequipTime;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		TSubclassOf<UDamageType> DamageType;

	/** The damage caused by a single shot. */
	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		float BaseDamage;

	/** The range at which the gun can be reliably shot to. In hitscan guns this is the maximu range of the gun.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		float WeaponRange;


	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		ECGAmmoType AmmoType;

	/** Modifies the spread while aiming.*/
	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		float ADSSpreadMod;

	FCGWeaponConfig()
	{
		// Timing.
		TimeBetweenShots = .01;
		bAutomaticFire = false;
		EquipTime = .05f;
		UnequipTime = .05f;

		// Attributes.
		bUsesProjectile = true;
		bHasManualReload = true;
		BaseDamage = 10.f;
		WeaponRange = 10000.f;

		OverHeatWeapon = false;

		AmmoType = ECGAmmoType::NONE;

		ADSSpreadMod = .5f;
	}
};

USTRUCT()
struct FCGAmmoData
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		int32 AmmoPerShot;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		int32 AmmoCapacity;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
		int32 ClipSize;

	/** The amount of time that the weapon is in the "Reload" state. */
	UPROPERTY(EditDefaultsOnly)
		float ReloadTime;

	FCGAmmoData()
	{
		AmmoPerShot = 1;
		AmmoCapacity = 100;
		ClipSize = 10;
		ReloadTime = .5f;
	}
};

USTRUCT()
struct FCGOverheatAmmoData
{
	GENERATED_USTRUCT_BODY()

		/** The maximum "heat" that the pistol can achieve before */
		UPROPERTY(EditDefaultsOnly)
		float MaxHeat;

	/** The heat generated by each shot. */
	UPROPERTY(EditDefaultsOnly)
		float HeatPerShot;

	/** Heat reduction per second.*/
	UPROPERTY(EditDefaultsOnly)
		float CooldownPerSecond;

	UPROPERTY(EditDefaultsOnly)
		float OverheatTime;


	/** Sets defaults */
	FCGOverheatAmmoData()
	{
		// Heat settings.
		MaxHeat = 100.f;
		HeatPerShot = 15.f;
		CooldownPerSecond = 30.f;
		OverheatTime = .5f;
	}
};


USTRUCT()
struct FCGProjectileData
{
	GENERATED_USTRUCT_BODY()

		/** The class of the Projectile that will be shot by the weapon. */
		UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ACGProjectile> ProjectileClass;

	/** The life span of the projectile before despawning. */
	UPROPERTY(EditDefaultsOnly)
		float ProjectileLife;

	/** Sets defaults */
	FCGProjectileData()
	{
		ProjectileClass = NULL;
		ProjectileLife = 2.0f;
	}
};

USTRUCT()
struct FCGSpreadData
{
	GENERATED_USTRUCT_BODY()

		/** The angle (in Degrees) of the base spread for this gun.*/
		UPROPERTY(EditDefaultsOnly)
		float BaseSpread;

	/** The angle (in Degrees) of the max spread for this gun.*/
	UPROPERTY(EditDefaultsOnly)
		float MaxSpread;

	/** The angle (in Degrees) that the gun spreads each continuous shot.*/
	UPROPERTY(EditDefaultsOnly)
		float SpreadPerShot;

	FCGSpreadData()
	{
		MaxSpread = 0.f;
		BaseSpread = 0.f;
		SpreadPerShot = 0.f;
	}
};

//FIXME this can be done with the seed and spread.
USTRUCT()
struct FCGInstantHit
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector Origin;

	UPROPERTY()
	FVector Direction;
};

/** VFX and SFX related to the weapon.*/
USTRUCT()
struct FCGWeaponFXData
{
	GENERATED_USTRUCT_BODY()

		////////////////////////////
		// VFX

		/** The socket on the gun model that corresponds to the muzzle of the gun. */
		UPROPERTY(EditDefaultsOnly, Category = Effects)
		FName MuzzleSocket;

	/** The Muzzle flash for the gun. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* MuzzleFlash;

	/** The name of the end of the trail in the material editor, should be in the target property.*/
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		FName TrailTargetParam;

	/** The Weapon trail for the bullet. Typically only used by hit scan weapons. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
		UParticleSystem* WeaponTrail;

	// TODO Improve impact.
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		UParticleSystem* ImpactEffect;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
		TSubclassOf<UCameraShake> CameraShake;


	////////////////////////////
	// SFX

	// XXX Maybe this should be a sound cue to prevent non prepped sounds from being used?
	/** Played on weapon fire. */
	UPROPERTY(EditDefaultsOnly, Category = Effects)
	class USoundBase* FireSound;

	FCGWeaponFXData()
	{
		/** The default name of the Muzzle socket. */
		MuzzleSocket = TEXT("MuzzleFlashSocket");
		TrailTargetParam = TEXT("TrailEnd");

	}
};

/** HUD Textures and configuration.*/
USTRUCT()
struct FCGWeaponHUDData
{
	GENERATED_USTRUCT_BODY()

		/**The crosshair icon for the weapon. Base color should be white. */
		UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon CrosshairIcon;

	/**The Ammo Guage Background for the weapon. Base color should be white. */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon AmmoGuageBGIcon;

	/**The Ammo Guage Foreground for the weapon. Base color should be white. */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon AmmoGuageFGIcon;

	/**The Weapon Icon for the weapon. Base color should be white. */
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FCanvasIcon WeaponIcon;

	/** Color for when the player has expended all of their available ammo for a clip.*/
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FLinearColor  LowAmmoColor;

	/** Color for when the player has expended none of their available ammo for a clip.*/
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		FLinearColor  FullAmmoColor;

	/** The width of the ammo clip in pixels*/
	UPROPERTY(EditDefaultsOnly, Category = HUD)
		float AmmoGuageWidth;

	FCGWeaponHUDData()
	{
		// Default colors for the ammo readout.
		LowAmmoColor = FLinearColor(1, 0, 0, 1); // Red
		FullAmmoColor = FLinearColor(0, 1, 0, 1); // Green

		AmmoGuageWidth = 256.0f;
	}
};

USTRUCT()
struct FCGHitInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Damage;

	UPROPERTY()
	uint32 bKillingHit: 1;

	UPROPERTY()
	TWeakObjectPtr<class ACGCharacter> Instigator;

	UPROPERTY()
	TWeakObjectPtr<class AActor> DamageCauser;
	// TODO learn how to do this damage event better.
	UPROPERTY()
	FDamageEvent DamageEvent;

private:
	UPROPERTY()
	uint8 ReplicationByte;

public:
	FCGHitInfo()
	{
		bKillingHit = false;
		Damage = 0;
	}
	void EnsureReplication()
	{
		ReplicationByte++;
	}
};

#pragma endregion

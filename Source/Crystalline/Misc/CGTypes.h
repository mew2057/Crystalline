// Fill out your copyright notice in the Description page of Project Settings.

#include "CGTypes.generated.h"

#pragma once


USTRUCT(BlueprintType)
struct FCGHUDRenderObject
{
	GENERATED_USTRUCT_BODY()

	/** The size of the element in pixels. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D Size;

	/** The position of the Element in pixels. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D Position;
	
	/** The anchor point for the RenderObject. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FVector2D Anchor;

	/** The color of the render element.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FLinearColor Color;

	FCGHUDRenderObject()
	{
		Color = FLinearColor::White;
	}
};

// A Text Binding.
DECLARE_DELEGATE_RetVal(FText, FCGGetTextDelegate)
USTRUCT(BlueprintType)
struct FCGHUDRenderText : public FCGHUDRenderObject
{
	GENERATED_USTRUCT_BODY()

	/** If this is bound The delegate is invoked. */
	FCGGetTextDelegate GetTextDelegate;

	/** The Text of the Element to be rendered.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	FText Text;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	UFont * Font;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderObject")
	float Scale;

	FCGHUDRenderText()
	{

	}
};


#pragma region Enums

UENUM(BlueprintType)
enum class ECGAmmoType : uint8
{
	NONE UMETA(DisplayName = "None"),
	T_ZERO UMETA(DisplayName = "Tier Zero"),
	T_ONE  UMETA(DisplayName = "Tier One"),
	T_TWO  UMETA(DisplayName = "Tier Two")
};
/** Implemented for TMap.*/
inline uint8 GetTypeHash(const ECGAmmoType A)
{
	return (uint8)A;
}

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

//TODO Move to its own class.
#pragma region Game Mode Structs
/**
* Base Class for predefined messages.
*/
USTRUCT(BlueprintType)
struct FCGMessage
{
	GENERATED_USTRUCT_BODY()

	/**The message that is displayed to the player.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	FString MessageText;

	/** The Sound effect associated with the message.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	USoundCue * MessageSound;

	FCGMessage()
	{
		
	}
};


/**
* Adds a Score to FCGMessage as a condition for displaying the message.
*/
USTRUCT(BlueprintType)
struct FCGScoreMessage : public FCGMessage
{
	GENERATED_USTRUCT_BODY()

	/**How far from the winning score should this message be played.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 PointsToWin;

	FCGScoreMessage()
	{

	}
};


/**
* Contains a collection of messages used to display relevant information to the player, based on the game mode.
*/
USTRUCT(BlueprintType)
struct FCGGameModeMessageProperties
{
	GENERATED_USTRUCT_BODY()

	/** The Game Mode that the messages are specified for.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	TSubclassOf<AGameMode> GameModeClass;

	// XXX Should this be a Hash Map?
	/**A Collection of score messages that give the player feedback on game state. Used by check score.*/
	UPROPERTY(EditDefaultsOnly, Category = Config)
	TArray<FCGScoreMessage> ScoreMessages;

	FCGGameModeMessageProperties()
	{

	}
};

/**
 * A Structure for organizing the messages that the game will display to the player.
 */
USTRUCT(BlueprintType)
struct FCGGameMessageProperties
{
	GENERATED_USTRUCT_BODY()
	// TODO generic messages.

	/** Colleciton of Messages that are game mode specific. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	TArray<FCGGameModeMessageProperties> GameModeMessageProperties;
	
private:

	/** The Index of the current game mode in the GameModeMessageProperties array.*/
	UPROPERTY(Transient)
	int32 CurrentGameModeIndex;

public:
	FCGGameMessageProperties() { }


	/**
	 * Copies the current game mode Message Properties to the supplied reference.
	 *
	 * @param OutMessageProps The message being set.
	 */
	void AssignMessageProperties(FCGGameModeMessageProperties& OutMessageProps)
	{
		if (CurrentGameModeIndex != -1)
		{
			OutMessageProps = GameModeMessageProperties[CurrentGameModeIndex];
		}
	}
	/**
	 * Determines the game mode index for the game from the supplied game mode class.
	 * Sets the CurrentGameModeIndex to -1 if the class was not specified.
	 *
	 * @param GameModeClass The Class that is being sought in the Properties.
	 */
	void SetCurrentGameMode(TSubclassOf<AGameMode> GameModeClass)
	{
		// Set the Game Mode to -1.
		CurrentGameModeIndex = -1;

		int32 Count = GameModeMessageProperties.Num();
		for (int32 i = 0; i < Count; ++i)
		{
			// If the GameMode matches set the index for future use.
			if (GameModeClass == GameModeMessageProperties[i].GameModeClass)
			{
				CurrentGameModeIndex = i;
				break;
			}
		}
	}
};
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

	FCGCrystalAmmo()
	{
		MaxAmmoCarried = 120;
		AmmoCarried = 80;
	}

};


USTRUCT()
struct FCGDDefaultCrystalTreeConfig
{
	GENERATED_USTRUCT_BODY()

	/** The Tier One Gun, Available when the player has a tier one crystal.*/
	UPROPERTY(EditDefaultsOnly, Category = Inventory)
	TSubclassOf<class ACGCrystalGun> TierOneGun;

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

public:
	UPROPERTY(EditDefaultsOnly, Category = Timing)
	uint32 bAutomaticFire : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	uint32 bUsesProjectile : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	uint32 bHasManualReload : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	uint32 bOverHeatWeapon : 1;

	UPROPERTY(EditDefaultsOnly, Category = WeaponAttributes)
	uint32 bHeadshotPossible : 1;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float TimeBetweenShots;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float EquipTime;

	UPROPERTY(EditDefaultsOnly, Category = Timing)
	float CrystalSlotTime;

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
		CrystalSlotTime = .05f;
		UnequipTime = .05f;

		// Attributes.
		bUsesProjectile = true;
		bHasManualReload = true;
		BaseDamage = 10.f;
		WeaponRange = 10000.f;

		bOverHeatWeapon = false;
		bHeadshotPossible = false;

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
	int32 ShotsPerClip;

	/** The amount of time that the weapon is in the "Reload" state. */
	UPROPERTY(EditDefaultsOnly)
	float ReloadTime;

	FCGAmmoData()
	{
		AmmoPerShot = 1;
		AmmoCapacity = 100;
		ShotsPerClip = 10;
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

/**Collects the animations for a weapon, defining both a first person and third person animation. */
USTRUCT()
struct FCGAnim
{
	GENERATED_USTRUCT_BODY()

	/** Animation to play from the first person view. */
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FirstPerson;

	/** Animation to play from the third person view. */
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ThirdPerson;
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

	/**Icon for hit confirmation.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon HitConfirmedIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FSlateBrush WeaponIconSlate;

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

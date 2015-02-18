// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "CGAmmo.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API UCGAmmo : public UObject
{
	GENERATED_BODY()
	friend class ACGCrystalGun;

public:
	UCGAmmo(const FObjectInitializer& ObjectInitializer);

	void PostInitializeComponents();

	void InitializeAmmo(const FCGCrystalAmmo& Config);
	
	FORCEINLINE int32 GetAmmoInClip() const { return AmmoInClip; }

	FORCEINLINE void AddAmmoToClip(const int32 Ammo)  
	{ 
		AmmoInClip += Ammo;
	}

	FORCEINLINE int32 GetAmmoCarried() const { return AmmoCarried; }

	/** Returns execess ammo. */
	FORCEINLINE int32 AddAmmoCarried(const int32 Ammo)  
	{ 
		AmmoCarried += Ammo; 
		return AmmoCarried - MaxAmmoCarried;
	}

	FORCEINLINE int32 GetMaxAmmoCarried() const { return MaxAmmoCarried; }

	// TODO make protected?
protected:
	UPROPERTY(EditDefaultsOnly, Category = Config)
	int32 MaxAmmoCarried;

	UPROPERTY(EditAnywhere, Category = Config)
	int32 AmmoCarried;

	UPROPERTY(EditAnywhere, Category = Config)
	int32 AmmoInClip;
	
	
};

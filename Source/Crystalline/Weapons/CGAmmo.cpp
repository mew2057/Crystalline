// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGAmmo.h"


UCGAmmo::UCGAmmo(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{

}

void UCGAmmo::PostInitializeComponents()
{

}

void UCGAmmo::InitializeAmmo(const FCGCrystalAmmo& Config)
{
	MaxAmmoCarried = Config.MaxAmmoCarried;
	AmmoCarried = Config.AmmoCarried;
	AmmoInClip = Config.AmmoInClip;	
}

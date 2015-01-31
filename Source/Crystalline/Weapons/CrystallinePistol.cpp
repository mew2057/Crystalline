// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CrystallinePistol.h"

void ACrystallinePistol::FireWeapon()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAttached(FireSound, GetRootComponent());
	}

}
// Fill out your copyright notice in the Description page of Project Settings.

#include "Crystalline.h"
#include "CGMarksmanRifle.h"


ACGMarksmanRifle::ACGMarksmanRifle(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	WeaponConfig.AmmoType = ECGAmmoType::T_ONE;
	HeadShotModifier = 2.f;
}

void ACGMarksmanRifle::DealDamage_Instant(const FHitResult& Impact, const FVector& ShootDir)
{
	ACGCharacter* HitPlayer = Cast<ACGCharacter>(Impact.GetActor());
	bool bHeadShot = HitPlayer && HitPlayer->IsHeadShot(Impact.Location, ShootDir);

	FPointDamageEvent PointDmg;
	PointDmg.DamageTypeClass = WeaponConfig.DamageType;
	PointDmg.HitInfo = Impact;
	PointDmg.ShotDirection = ShootDir;
	PointDmg.Damage = bHeadShot ? HeadShotModifier * WeaponConfig.BaseDamage : WeaponConfig.BaseDamage;

	if (bHeadShot)
	{
		UE_LOG(LogTemp, Log, TEXT("HEAD SHOT!"));
	}

	Impact.GetActor()->TakeDamage(PointDmg.Damage, PointDmg, CGOwner->Controller, this);
}
// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "CrystallineProjectile.generated.h"

UCLASS(Abstract,config=Game)
class ACrystallineProjectile : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComp;

public:
	ACrystallineProjectile(const FObjectInitializer& ObjectInitializer);
	
	virtual void PostInitializeComponents() override;

	/** called when projectile hits something */
	UFUNCTION()
	void OnImpact(const FHitResult& Hit);

	/** Sets the velocity of the projectile*/
	UFUNCTION()
	void SetVelocity(FVector Direction);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return MovementComp; }

	/** update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
};


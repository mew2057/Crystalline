// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Actor.h"
#include "CrystallineProjectile.generated.h"

UCLASS(Abstract,config=Game)
class ACrystallineProjectile : public AActor
{
	GENERATED_UCLASS_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComp;

	/** Set to true when the Projectile impacts something. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Impacted)
	uint32 bImpacted : 1;

public:
	
	virtual void PostInitializeComponents() override;

	/** Called when projectile hits something */
	UFUNCTION()
	void OnImpact(const FHitResult& Hit);

	/** Sets the velocity of the projectile*/
	UFUNCTION()
	void SetVelocity(FVector Direction);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return MovementComp; }

	////////////////////////////////////////////////////////
	// Functions expected to be overrided

	/** Update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;
	
	/** Processes the impact with projectile specific implementation details. */
	virtual void ProcessImpact(const FHitResult& Hit);

	/** Cleans up the projectile for destruction, default behavior stops then destroys the projectile.*/
	virtual void PrepForDestroy();

	////////////////////////////////////////////////////////
	// Replication
	UFUNCTION()
	void OnRep_Impacted();
};


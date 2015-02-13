// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGProjectile.generated.h"

/**
 * 
 */
UCLASS(Abstract, config = Game)
class CRYSTALLINE_API ACGProjectile : public AActor
{
	GENERATED_BODY()
public:

protected:
	/** Sphere collision component */
	UPROPERTY( EditDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComp;

	/** Set to true when the Projectile impacts something. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Impacted)
	uint32 bImpacted : 1;

public:
	UPROPERTY()
	float ImpactDamage;

	ACGProjectile(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	/** Called when projectile hits something */
	UFUNCTION()
	void OnImpact(const FHitResult& Hit);

	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


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

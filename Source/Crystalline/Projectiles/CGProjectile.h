// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGProjectile.generated.h"

/**
 * Defines damage dealing and initialization for projectiles, e.g. pistol bullets, shotgun bullets, etc.
 */
UCLASS(Abstract, config = Game)
class CRYSTALLINE_API ACGProjectile : public AActor
{
	GENERATED_BODY()
public:
	//TODO
#pragma region ReplaceWithStruct
	/**Impact damage.*/
	float ImpactDamage;

	/**Type of Impact Damage*/
	TSubclassOf<UDamageType> DamageType;
#pragma endregion

	ACGProjectile(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnStop(const FHitResult& Hit);
	
	/** Sets the velocity of the projectile*/
	UFUNCTION()
	void SetVelocity(FVector Direction);

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return MovementComp; }

	////////////////////////////////////////////////////////
	// Functions expected to be overriden

	/** Update velocity on client */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	/** Processes the impact with projectile specific implementation details. */
	virtual void ProcessImpact(const FHitResult& Hit);

	/** Cleans up the projectile for destruction, default behavior stops then destroys the projectile.*/
	virtual void PrepForDestroy();

	/**Spawns the trail for the projectile.*/
	virtual void SpawnTrailParticleSystem();


	/** 
	 * Spawns the impact visuals for the collision. 
	*/
	virtual void SpawnImpact();
	
protected:
	/** Sphere collision component. */
	UPROPERTY( EditDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* MovementComp;

	/** The Weapon trail for the projectile. Each projectile should do this differently. */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	UParticleSystem* ProjectileTrail;

	/** The Particle system that is played when the projectile collides with another object. */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	UParticleSystem* ImpactEffect;

	/** The trail that the projectile leaves. This is like a tracer round. */
	UPROPERTY(Transient)
	UParticleSystemComponent* TrailPSC;
};
 
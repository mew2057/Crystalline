// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGProjectile.generated.h"

#define CG_PROJECTILE_IMPACT_LIFESPAN .25f

/** XXX This is a simple solution to the problem of projectiles, but it is not as reliable as a "real" projectile system. It needs more client side support to really look right, but I just don't have the time/knowledge to do that at the moment -John.
 * Defines damage dealing and initialization for projectiles, e.g. pistol bullets, shotgun bullets, etc.
 */
UCLASS(Abstract, config = Game)
class CRYSTALLINE_API ACGProjectile : public AActor
{
	GENERATED_BODY()
public:
	ACGProjectile(const FObjectInitializer& ObjectInitializer);

	/**Spawns the Trail for the */
	virtual void PostInitializeComponents() override;

	/** Update velocity on client. */
	virtual void PostNetReceiveVelocity(const FVector& NewVelocity) override;

	UFUNCTION()
	void OnStop(const FHitResult& Hit);
	
	/** 
	 * Sets the velocity of the MovementComponent, uses the supplied direction with the initial speed of the movement component to calculate the velocity. 
	 *
	 * @param Direction The Direction the bullet should be moving.
	 */
	UFUNCTION()
	void SetVelocity(FVector Direction);

	/** 
	 * Processes the impact with projectile specific implementation details. 
	 *
	 * @param Hit The point at which the projectile collided with another Actor in the scene, pawn or otherwise.
	 */
	void ProcessImpact(const FHitResult& Hit);

	/**
	 * Retrieves the Point Damage for a projectile, Override for damage falloff or weapon specific mechanics.
	 *
	 * @return The damage caused by the projectile colliding with a pawn.
	*/
	virtual float GetPointDamage();

	/** 
	 * Cleans up the projectile for destruction, default behavior stops then destroys the projectile after .25 seconds.
	 */
	virtual void PrepForDestroy();

	/**
	 * Spawns the trail for the projectile. Deactivates on impact.
	 */
	virtual void SpawnTrailParticleSystem();

	/** 
	 * Spawns the impact visuals for the collision. 
	*/
	virtual void SpawnImpact();
	
	/** Damage caused to a player that is hit by the projectile. */
	UPROPERTY(Transient)
	float ImpactDamage;

	/** The Damage Type of the projectile, this is used in dealing damage. */
	UPROPERTY(Transient)
	TSubclassOf<UDamageType> DamageType;

	/** @return The Collision component of the projectile. **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** @return the Movement Component of the projectile. **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return MovementComp; }
	
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
 
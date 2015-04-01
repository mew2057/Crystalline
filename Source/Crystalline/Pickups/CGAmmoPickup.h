// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Misc/CGTypes.h"
#include "CGAmmoPickup.generated.h"

/** XXX Is this properly replicated?
 * Defines an ammo pickup, typically dropped by a player.
 */
UCLASS()
class CRYSTALLINE_API ACGAmmoPickup : public AActor
{
	GENERATED_BODY()
public:
	ACGAmmoPickup(const FObjectInitializer& ObjectInitializer);

	/** 
	 * Gives the ammo to the player if collided.
	 */
	virtual void ReceiveActorBeginOverlap(class AActor* Other) override;

	/**
	 * Clears the timer handles and invokes Destroy.
	 */
	void ProcessDestroy();

	/** 
	 * Initializes the ammo attributes and ignores the actor that spawned it.
	 * This should be called after the AmmoPickup has been spawned.
	 *
	 * @param Owner The Actor that is spawning the AmmoPickup. This prevents the pickup from being grabbed by a ragdoll.
	 * @param BaseAmmo The original amount of ammo held by this pickup.
	 * @param BaseAmmoType The type of ammo this pickup should have.
	 */
	void Initialize(class AActor* Owner, int32 BaseAmmo, ECGAmmoType BaseAmmoType);

protected:
	/** The amount of ammo in the pickup.*/
	int32 Ammo;

	/** The type of ammo that this weapon supplies.*/
	ECGAmmoType AmmoType;

	/** The lifespan of the pickup.*/
	float LifeSpan;

	/** Defines the visual component for the ammo pickup.*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* AmmoMesh;

	/**Timer Handle for the Ammo Destroy.*/
	FTimerHandle TimerHandle_Destroy;
};

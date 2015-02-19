// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"


#include "CGAmmoPickup.generated.h"


/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGAmmoPickup : public AActor
{
	GENERATED_BODY()
public:
	ACGAmmoPickup(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	/** Gives the ammo to the player if collided. */
	virtual void ReceiveActorBeginOverlap(class AActor* Other) override;

	void ProcessDestroy();

	/** Initializes the ammo attributes and ignores the actor that spawned it.*/
	void Initialize(class AActor* Owner, int32 BaseAmmo, ECGAmmoType BaseAmmoType);

protected:
	int32 Ammo;

	ECGAmmoType AmmoType;

	/**The lifespan of the pickup.*/
	float LifeSpan;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* AmmoMesh;

};

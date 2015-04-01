// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Misc/CGTypes.h"
#include "CGCrystal.generated.h"

/**
 * The Actor representing the power up crystals in the game world.
 */
UCLASS()
class CRYSTALLINE_API ACGCrystal : public AActor
{
	GENERATED_BODY()
public:
	ACGCrystal(const FObjectInitializer& ObjectInitializer);

	/** Despawns the crystal if it was flaged as not present on start.*/
	virtual void PostInitializeComponents() override;

	/** Triggers the pickup prompt for the crystal. */
	virtual void ReceiveActorBeginOverlap(class AActor* Other) override;
	
	/** Removes the pickup prompt for the crystal. */
	virtual void ReceiveActorEndOverlap(class AActor* Other) override;

	/** 
	 * Invoked by the Actor when they want to pickup the crystal.
	 * 
	 * @return True if the crystal was successfully picked up.
	 */
	bool Pickup();
	
	/** Invoked once the respawn timer is completed.*/
	void OnRespawn();

	/** Hides the crystal and sets it's state to inactive. */
	void HideCrystal();

	/** Shows the crystal and sets it's state to active. */
	void ShowCrystal();

	/** Hides or shows the crystal based upon it's activity state. */
	UFUNCTION()
	void OnRep_Active();

	/** 
	 * Getter for the crystal type.
	 *
	 * @return The Type of crystal this pickup represents.
     */
	FORCEINLINE ECGCrystalType GetCrystalType() const { return CrystalType; };

protected:

	/** The Crystal type of the pickup (Power, Range, Utility, None). */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	ECGCrystalType CrystalType;

	/** Crystal Mesh, defines the visual component of the crystal.*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* CrystalMesh;

	// XXX maybe this should be a blueprint defined element?
	/** Base Mesh, defines the visual component that the crystal is on (if any).*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* BaseMesh;

	/**The overlap volume, if the player collides with this they may grab the crystal. */
	UPROPERTY(VisibleDefaultsOnly, Category = Config)
	UCapsuleComponent* OverlapVolume;

	/** Tracks whether or not the crystal can be picked up. */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Active)
	uint32 bIsActive : 1;

	/**If set to true the crystal will spawn as though it were present on the construction of the object. */
	UPROPERTY(EditAnywhere, Category = Config)
	uint32 bSpawnActive : 1;

	/** Time to respawn after pickup, if set to zero the crystal is never exhausted. */
	UPROPERTY(EditAnywhere, Category = Config)
	float TimeToRespawn;

	/** Timer Handle for the Respawn timer.*/
	FTimerHandle TimerHandle_Respawn;
};

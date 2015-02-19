// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGCrystal.generated.h"


UENUM(BlueprintType)
enum class ECrystalType : uint8
{
	NONE UMETA(DisplayName = "None"),
	POWER_UP UMETA(DisplayName = "Power Up"),
	FORCE UMETA(DisplayName = "Force"),
	ACCURACY  UMETA(DisplayName = "Accuracy"),
	UTILITY	 UMETA(DisplayName = "Utility")
};

// TODO find out what the overhead on this is.
/** Implemented for TMap.*/
inline uint8 GetTypeHash(const ECrystalType A)
{
	return (uint8)A;
}

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGCrystal : public AActor
{
	GENERATED_BODY()
public:
	ACGCrystal(const FObjectInitializer& ObjectInitializer);
	void PostInitializeComponents();

	/** Triggers the pickup prompt for the crystal. */
	virtual void ReceiveActorBeginOverlap(class AActor* Other) override;
	
	/** Removes the pickup prompt for the crystal. */
	virtual void ReceiveActorEndOverlap(class AActor* Other) override;

	/** Invoked by the actor, returns false if the pickup is invalid.*/
	bool Pickup();
	
	/** Invoked once the respawn timer is done.*/
	void OnRespawn();

	/**Hides the crystal and sets it's state to inactive. */
	void HideCrystal();

	/** Shows the crystal and sets it's state to active. */
	void ShowCrystal();

	UFUNCTION()
	void OnRep_Active();

	FORCEINLINE ECrystalType GetCrystalType() const { return CrystalType; };

protected:

	/** The Crystal type of the pickup. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	ECrystalType CrystalType;

	/** Crystal Mesh, defines the Noteworthy portion of the crystal.*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* CrystalMesh;

	/** Base Mesh, defines Base that the crystal is on (if any).*/
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

};

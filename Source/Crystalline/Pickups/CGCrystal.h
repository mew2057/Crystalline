// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "CGCrystal.generated.h"


UENUM(BlueprintType)		
enum class ECrystalType : uint8
{
	UPGRADE UMETA(DisplayName = "Upgrade"),
	FORCE UMETA(DisplayName = "Force"),
	ACCURACY  UMETA(DisplayName = "Accuracy"),
	UTILITY	 UMETA(DisplayName = "Utility")
};


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
	virtual void ACGCrystal::ReceiveActorBeginOverlap(class AActor* Other) override;
	
	/** Removes the pickup prompt for the crystal. */
	virtual void ACGCrystal::ReceiveActorEndOverlap(class AActor* Other) override;

	/** Invoked by the actor, returns false if the pickup is invalid.*/
	bool OnDespawn();

	/** Syncronizes the status of the crystal. */
	UFUNCTION(reliable, server, WithValidation)
	void ServerOnDespawn();

	void OnRespawn();

	/** Syncronizes the status of the crystal. */
	UFUNCTION(reliable, server, WithValidation)
	void ServerOnRespawn();

	/**Hides the crystal and sets it's state to inactive. */
	void HideCrystal();

	/** Shows the crystal and sets it's state to active. */
	void ShowCrystal();

	UFUNCTION()
	void OnRep_Active();


protected:
	/** Crystal Mesh, defines the Noteworthy portion of the crystal.*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* CrystalMesh;

	/** Base Mesh, defines Base that the crystal is on (if any).*/
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	UMeshComponent* BaseMesh;

	/**The overlap volume, if the player collides with this they may grab the crystal. */
	UPROPERTY(VisibleDefaultsOnly, Category = Config)
	UCapsuleComponent* OverlapVolume;

	/** The Crystal type of the pickup. */
	UPROPERTY(EditDefaultsOnly, Category = Config)
	ECrystalType CrystalType;
	
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

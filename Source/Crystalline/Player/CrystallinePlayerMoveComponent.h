// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "CrystallinePlayer.h"

#include "CrystallinePlayerMoveComponent.generated.h"

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API UCrystallinePlayerMoveComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	virtual float GetMaxSpeed() const override;
	
};

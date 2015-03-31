// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/DamageType.h"
#include "CGDamageType.generated.h"

/**
 * 
 */
UCLASS(const, Blueprintable, BlueprintType)
class CRYSTALLINE_API UCGDamageType : public UDamageType
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = HUD)
	FCanvasIcon KillIcon;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UForceFeedbackEffect *HitForceFeedback;

	UPROPERTY(EditDefaultsOnly, Category = Effects)
	UForceFeedbackEffect *KilledForceFeedback;
	
};

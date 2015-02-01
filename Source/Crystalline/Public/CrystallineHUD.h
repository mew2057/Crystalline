// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "CrystallineHUD.generated.h"

#define TARGET_Y_RESOLUTION 1080.0f
#define TARGET_X_RESOLUTION 1920.0f

UCLASS()
class ACrystallineHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACrystallineHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	

private:
	/** The vertical scale factor of the UI Relative to 1080.*/
	uint32 ScaleUIY;

	/** The horizontal scale factor of the UI Relative to 1920.*/
	//uint32 ScaleUIX; Commented for potential future usage. -John

};


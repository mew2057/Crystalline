// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "CrystallineHUD.generated.h"

UCLASS()
class ACrystallineHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACrystallineHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:

};


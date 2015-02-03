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

	/** 
	 * Draws the ammo, overheat gauge, and other relavant information.
	 * Note this assumes that the player only has two weapons, more weapons require modifications.
	 */
	void DrawWeaponHUD(const ACrystallinePlayer* Pawn);

	/** Draws the player's current health on the screen.*/
	void DrawHealth(const ACrystallinePlayer* Pawn);

	/** Draws Information regarding the current game type.*/
	void DrawGameInfo(const ACrystallinePlayer* Pawn);


private:
	/** The vertical scale factor of the UI Relative to 1080.*/
	float ScaleUIY;

	/** The horizontal scale factor of the UI Relative to 1920.*/
	//float ScaleUIX; Commented for potential future usage. -John

};


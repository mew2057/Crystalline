// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/HUD.h"
#include "CGPlayerHUD.generated.h"

#define TARGET_Y_RESOLUTION 1080.0f
#define TARGET_X_RESOLUTION 1920.0f
#define ICON_FUDGE .0000001f

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API ACGPlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACGPlayerHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	/**
	* Draws the ammo, overheat gauge, and other relavant information.
	* Note this assumes that the player only has two weapons, more weapons require modifications.
	*/
	void DrawWeaponHUD();

	/** Draws the player's current health on the screen.*/
	void DrawHealth();

	/** Draws Information regarding the current game type.*/
	void DrawGameInfo();


	/** Draws the prompt message.*/
	void DrawPrompt();

	void SetPromptMessage(const FString& Message);

	UPROPERTY(EditDefaultsOnly, Category = FontSettings)
		UFont* BigFont;

private:
	/** The vertical scale factor of the UI Relative to 1080.*/
	float ScaleUIY;

	/** The horizontal scale factor of the UI Relative to 1920.*/
	//float ScaleUIX; Commented for potential future usage. -John

	
	
	
};

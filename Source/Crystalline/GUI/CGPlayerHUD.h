// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/HUD.h"
#include "CGPlayerHUD.generated.h"

#define TARGET_Y_RESOLUTION 1080.0f
#define TARGET_X_RESOLUTION 1920.0f
#define ICON_FUDGE .0000001f
USTRUCT()
struct FCGHUDTransform
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float WidthPercent;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float HeightPercent;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float PercentX;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float PercentY;

	FCGHUDTransform()
	{
		WidthPercent  = 0;
		HeightPercent = 0;
		PercentX	  = 0;
		PercentY	  = 0;
	}
};

USTRUCT()
struct FCGHUDElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon FGIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon BGIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor FullColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor EmptyColor;

	FCGHUDElement()
	{
		FullColor = FLinearColor::Blue;
		EmptyColor = FLinearColor::Red;
	}
};

USTRUCT()
struct FCGGameElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;	

	FCGGameElement()
	{
	
	}


};

USTRUCT()
struct FCGRoundElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform TimeTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor TimeColor;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform ScoreTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor ScoreColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon FGIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon BGIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon OwnerIcon;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor FullColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor EmptyColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	TArray<FCGGameElement> GameDataElements;

	FCGRoundElement()
	{
		TimeColor = FLinearColor::White;
		ScoreColor = FLinearColor::White;
		FullColor = FLinearColor::Blue;
		EmptyColor = FLinearColor::Red;
	}
};

USTRUCT()
struct FCGWeaponElement
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform InClipAmmoTransform;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform HeldAmmoTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor AmmoTextColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform GuageTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform MainIconTransform;


	FCGWeaponElement()
	{
		AmmoTextColor = FLinearColor::White;
	}

};


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
	virtual void PostRender() override;
	virtual void PostInitializeComponents() override;

	/**
	* Draws the ammo, overheat gauge, and other relavant information.
	* Note this assumes that the player only has two weapons, more weapons require modifications.
	*/
	void DrawWeaponHUD();

	/** Draws the player's current shield on the screen.*/
	void DrawShield();

	/** Draws Information regarding the current game type.*/
	void DrawGameInfo();

	/**Draws text with the specified height.*/
	FORCEINLINE void DrawScaledText(const FString & Text, FLinearColor TextColor, float ScreenX, float ScreenY, UFont * Font, float TextHeight);

	/** Draws the prompt message.*/
	void DrawPrompt();

	void SetPromptMessage(const FString& Message);

private:
	/** The vertical scale factor of the UI Relative to 1080.*/
	float ScaleUIY;

	/** The horizontal scale factor of the UI Relative to 1920.*/
	//float ScaleUIX; Commented for potential future usage. -John


	/** The Prompt Message, In the future this shouold be a struct.*/
	FString PromptMessage;

	/**The number of pixels per percent.*/
	FVector2D PixelsPerCent;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDElement Shield;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGRoundElement RoundDataElement;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGWeaponElement WeaponElement;

	UPROPERTY(EditDefaultsOnly, Category = FontSettings)
	UFont* BigFont;

	UPROPERTY(Transient)
	float ScoreToWinInv;
};

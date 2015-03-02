// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "GameFramework/HUD.h"
#include "CGPlayerHUD.generated.h"

#define TARGET_Y_RESOLUTION 1080.0f
#define TARGET_X_RESOLUTION 1920.0f
#define ICON_FUDGE .0000001f
#define ALPHA_BUTTON_ICON_OFFSET 65
#define NUM_BUTTON_OFFSET 25

#pragma region Structs
UENUM(BlueprintType)
enum class ECGJustification : uint8
{
	LEFT   UMETA(DisplayName = "Left"),
	CENTER UMETA(DisplayName = "Center"),
	RIGHT  UMETA(DisplayName = "Right")
};

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
	FLinearColor DataElementColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform TimeTransform;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float TimeAnchor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor TimeColor;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform ScoreTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor ScoreColor;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float ScoreAnchor;

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
		DataElementColor = FLinearColor::White;
		TimeColor = FLinearColor::White;
		ScoreColor = FLinearColor::White;
		FullColor = FLinearColor::Blue;
		EmptyColor = FLinearColor::Blue;
		TimeAnchor = 0.f;
		ScoreAnchor = 0.f;
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
	float InClipAnchor;
	
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform HeldAmmoTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float HeldAmmoAnchor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor AmmoTextColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform GuageTransform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform MainIconTransform;


	FCGWeaponElement()
	{
		AmmoTextColor = FLinearColor::White;
		InClipAnchor   = 0.f;
		HeldAmmoAnchor = 0.f;

	}
};

#define NUM_MAPPED_BUTTONS 2
#define ACTION_BUTTON 0
#define POP_BUTTON 1

USTRUCT()
struct FCGButtonIcons
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	UTexture2D* ButtonIconTexture;


	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	int32 IconWidth;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	int32 IconHeight;
	
	int32 IconsX;

	TArray<FVector2D> ButtonIcons;

	FCGButtonIcons()
	{
		IconWidth  = 64;
		IconHeight = 64;

		// TODO remove hardcoding.
		ButtonIcons.Init(NUM_MAPPED_BUTTONS);
	}

	void Initialize()
	{
		if (ButtonIconTexture != NULL)
		{
			IconsX = ButtonIconTexture->GetSizeX() / IconWidth;
		}
	}
	
	void SetKeyboardIcon(int32 Position, int32 ButtonID)
	{	
		if (IconsX > 0)
		{
			int32 Row = Position % IconsX;
			int32 Col = Position / IconsX;

			
			ButtonIcons[ButtonID].X = (float)(Row *  IconWidth);
			ButtonIcons[ButtonID].Y = (float)(Col *  IconHeight);
		}
	}
};




USTRUCT()
struct FCGPrompt
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FString BasePrompt;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor PromptTextColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float PromptKeyOffset;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float Anchor;

	/** The Prompt Message.*/
	FString PromptMessage;

	/** The UV for the prompt on the HUD Icons.*/
	FVector2D PromptIconUV;

	uint32 bPrompt : 1;

	/**The Index of the current button in the prompt.*/
	int32 CurrentButton;

	FCGPrompt()
	{
		BasePrompt = "Press";
		PromptMessage = "";
		PromptTextColor = FLinearColor::White;
		bPrompt = false;
		Anchor = .5f;
	}
};


USTRUCT()
struct FCGScoreboardElement
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCanvasIcon RowBackground;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor HeaderBackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor RowBackgroundColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor OwnerBackgroundColor;

	// Transform of the scoreboard "Box"
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float Alignment;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float RowHeightPercent;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float RowOffsetPercent;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float ColumnOffsetPercent;
	
	// Alignments.

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float RankAlignment;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float DesiredRankWidth;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float NameAlignment;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float DesiredNameWidth;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float ScoreAlignment;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float DesiredScoreWidth;

	// Color.
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor TextColor;

	FCGScoreboardElement()
	{
		Transform.WidthPercent = 20.f;
		Transform.HeightPercent = 75.f;
		Transform.PercentX = 50.f;
		Transform.PercentY = 20.f;

		Alignment = .5f;
	
		HeaderBackgroundColor = FLinearColor(.15f, 0.f, .0f);
		OwnerBackgroundColor = FLinearColor(0.f, 0.f, .15f);
		RowBackgroundColor = FLinearColor(.15f, .15f, .15f);

		TextColor = FLinearColor::White;

		RowHeightPercent = 4.f;
		RowOffsetPercent = 1.f;
		ColumnOffsetPercent = 2.5f;

		RankAlignment = .5f;
		NameAlignment = 0.03f;
		ScoreAlignment = 1.f;

		DesiredRankWidth = 15.f;
		DesiredNameWidth = 50.f;
		DesiredScoreWidth = 25.f;
	}
};

USTRUCT()
struct FCGEndGameMessage
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGHUDTransform Transform;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FLinearColor TextColor;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	float Alignment;

	UPROPERTY(EditDefaultsOnly, Category = FontSettings)
	UFont* Font;

	UPROPERTY(Transient)
	FString Message;

	uint32 bDisplay:1;

	FCGEndGameMessage()
	{
		Transform.PercentX = 50.f;
		Transform.PercentY = 11.f;
		Transform.HeightPercent = 8.f;

		TextColor = FLinearColor::White;
		Message = "";
		bDisplay = false;
		Alignment = .5f;
	}
}; 


#pragma endregion
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

	void DetermineKeyCodeForAction(const FName& Action, int32 ButtonID, bool GamepadConnected);

	/**
	* Draws the ammo, overheat gauge, and other relavant information.
	* Note this assumes that the player only has two weapons, more weapons require modifications.
	*/
	void DrawWeaponHUD();

	/** Draws the player's current shield on the screen.*/
	void DrawShield();

	/** Draws Information regarding the current game type.*/
	void DrawGameInfo();

	/**Draws text with the specified height.
	@return The Horizontal scale.*/
	FORCEINLINE float DrawScaledText(const FString & Text, FLinearColor TextColor, float ScreenX, float ScreenY, UFont * Font, float TextHeight, float Anchor = 0.f);


	/** Draws the prompt message.*/
	void DrawPrompt();

	UFUNCTION(BlueprintCallable, Category = "Game|HUD")
	void SetPromptMessage(bool bSetPrompt, const FString& Message = "", int32 ButtonID = 0);

	/**Sets the TimeSinceLastHit for the hit notification.*/
	void NotifyHitTaken();

	void NotifyHitConfirmed();


	FORCEINLINE void SetScoreboardVisibility(bool bVisible){ bScoreboardVisible = bVisible; }
	FORCEINLINE void ToggleScoreboardVisibility(){ bScoreboardVisible = !bScoreboardVisible; }

	void DrawScoreboard();

	UFUNCTION(BlueprintCallable, Category = "Game|HUD")
	void SetEndGameMessage(bool bSetMessage, const FString& Message = "");

	/** Draws the prompt message.*/
	void DrawEndGameMessage();

private:
	/** The vertical scale factor of the UI Relative to 1080.*/
	float ScaleUIY;

	/** The horizontal scale factor of the UI Relative to 1920.*/
	//float ScaleUIX; Commented for potential future usage. -John


	/** The Prompt Message, In the future this shouold be a struct.*/
	FString PromptMessage;

	/**The number of pixels per percent.*/
	FVector2D PixelsPerCent;

	/**The configuration for the Shield HUD Element.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FCGHUDElement Shield;

	/**The configuration for the Round HUD Elements.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FCGRoundElement RoundDataElement;

	/**The configuration for the Weapon HUD Elements.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FCGWeaponElement WeaponElement;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FCGButtonIcons ButtonIcons;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FCGPrompt Prompt;

	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGScoreboardElement Scoreboard;

	/**Defines where the endgame message is displayed.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
	FCGEndGameMessage EndGameMessage;

	/** Expanded to fit across the player's FOV. */
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		UTexture2D* HitTakenOverlay;

	/**The color for the hit taken texture.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FLinearColor HitTakenColor;

	/**Time to display the hit taken texture.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		float TimeToDisplayHitTaken;

	/**Icon for hit confirmation.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		FCanvasIcon HitConfirmedIcon;

	/**Time to display the hit confirmation.*/
	UPROPERTY(EditDefaultsOnly, Category = HUDElements)
		float TimeToDisplayHitConfirmed;

	/**The Font for the HUD.*/
	UPROPERTY(EditDefaultsOnly, Category = FontSettings)
		UFont* BigFont;

	/**Internal time since the player last took a hit.*/
	UPROPERTY(Transient)
		float TimeSinceLastHitTaken;

	/**Time Since the last hit was confirmed.*/
	UPROPERTY(Transient)
		float TimeSinceLastHitConfirmed;

	uint32 bScoreboardVisible : 1;


};

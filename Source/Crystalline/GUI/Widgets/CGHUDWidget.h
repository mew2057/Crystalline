// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "CanvasTypes.h"
#include "CGHUDWidget.generated.h"
struct FCGCanvasTextItem : public FCanvasTextItem
{
	FCGCanvasTextItem(
		const FVector2D& InPosition, 
		const FText& InText, 
		class UFont* InFont, 
		const FLinearColor& InColor)
		: FCanvasTextItem(InPosition, InText, InFont, InColor)
	{}

	virtual void Draw(class FCanvas* InCanvas) override;

};

/**
 * 
 */
UCLASS()
class CRYSTALLINE_API UCGHUDWidget : public UObject
{
	GENERATED_BODY()
	
	
	
	
};

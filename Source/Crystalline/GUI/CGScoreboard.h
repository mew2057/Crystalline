// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CGPlayerHUD.h"
#include "SlateBasics.h"

/**
 * 
 */
class SCGScoreboardWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SCGScoreboardWidget)
	{}
	SLATE_ARGUMENT(TWeakObjectPtr<class ACGPlayerHUD>, OwnerHUD)

	SLATE_END_ARGS()
public:
	/** Mandatory in widgets.*/
	void Construct(const FArguments& InArgs);

private:
	TWeakObjectPtr<class ACGPlayerHUD> OwnerHUD;

};

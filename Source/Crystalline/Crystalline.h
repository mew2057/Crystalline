// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#ifndef __CRYSTALLINE_H__
#define __CRYSTALLINE_H__

#include "Engine.h"
#include "TimerManager.h"
#include "UnrealNetwork.h"
#include "Online/CGGameSession.h"
#include "GUI/CGPlayerHUD.h"
#include "Player/CGCharacter.h"
#include "Player/CGInventory.h"
#include "Weapons/CGWeapon.h"
#include "Weapons/CGCrystalGun.h"

#define COLLISION_PROJECTILE ECC_GameTraceChannel1
#define COLLISION_WEAPON     ECC_GameTraceChannel2
#define COLLISION_AMMO_DROP  ECC_GameTraceChannel3

/** Score Goal to win (e.g. kills )*/
#define GM_SCORE_GOAL	 FString(TEXT("gs"))
/** Time Limit for the game.*/
#define GM_TIME_LIMIT	 TEXT("tl")
/** Score Removed by a death. */
#define GM_DEATH_PENALTY TEXT("dp")
/** Score Given by a kill. */
#define GM_KILL_SCORE	 TEXT("ks")
/** Score Given by an objective. */
#define GM_KILL_SCORE	 TEXT("os")




#endif

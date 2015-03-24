#include "Crystalline.h"
#include "CGPlayerStart.h"

bool ACGPlayerStart::IsInitialSpawn() 
{ 
	return bInitialSpawnOnly; 
}

bool ACGPlayerStart::IsBotSpawn()
{ 
	return bBotsMaySpawn; 
}

uint8 ACGPlayerStart::GetTeamNumber()
{ 
	return TeamNumber; 
}
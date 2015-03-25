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

float ACGPlayerStart::GetLastSpawnTime()
{
	return LastSpawnTime;
}

void ACGPlayerStart::SetLastSpawnTime(float Time)
{
	LastSpawnTime = Time;
}

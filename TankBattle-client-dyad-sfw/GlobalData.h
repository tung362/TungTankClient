#pragma once
#include "ClientFunctions.h"
#include "TankBattleNet.h"
#include "sfwdraw.h"
#include "Vector2.h"
#include <iostream>
#include <vector>
using std::vector;

struct ForbiddenZone
{
	Vector2 Position;
};

enum AIState
{
	Wander, Pursue, Avoid
};

struct GlobalData
{
	//Settings//
	AIState CurrentAIState;
	int ActualWindowWidth;
	int ActualWindowHeight;
	int WindowWidth;
	int WindowHeight;
	int MyPlayerID;
	char * ServerIPAddress;

	//AI
	bool ManualOverride;
	bool ReachedDestination;
	float ClosestDistanceToDestination;
	Vector2 TargetDestination;

	float backingupTimer;
	float backingupDuration;
	bool startBackingup;

	float forceForwardTimer;
	float forceForwardDuration;
	bool startForcedForward;
	float forceHalfTurnTimer;
	float forceHalfTurnDuration;
	bool startHalfTurn;
	float PreviousTankForward;

	float previousHealth;
	float updateHealthTimer;
	float updateHealthDelay;
	bool startUpdatingHealth;

	int CurrentEnemySeen;
	int PreviousEnemySeen;

	float GoToFixedDestinationTimer;
	float GoToFixedDestinationDuration;
	bool GoToFixedDestination;

	float CancelTargetDirectionSearchTimer;
	float CancelTargetDirectionSearchDuration;
	bool StartCancelTargetDirectionSearch;
	float TargetLastDirectionUpdateElapsed[3];
	Vector2 PreviousTargetDirection[3];

	//Invalid Position Fix
	float AngleOffsetResetTimer;
	float AngleOffsetResetDuration;
	bool StartAngleOffset;
	float AngleOffset;

	//Textures//
	unsigned Font;

	//Update//
	tankNet::TankBattleStateData * State;

	//Trackers
	vector<ForbiddenZone> ForbiddenZones;

	//Minimap
	float HALF_WIDTH;
	float HALF_HEIGHT;
	unsigned g_DMAP;
	unsigned g_DOTS;
};
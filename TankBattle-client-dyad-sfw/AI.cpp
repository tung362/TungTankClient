#include "AI.h"
#include "ClientFunctions.h"
#include "GlobalData.h"
#include "TankBattleNet.h"
#include "sfwdraw.h"
#include "Vector2.h"
#include <iostream>
#include <sstream>
#include <cmath>
#undef NONE

using std::cout;
using std::endl;
using namespace tankNet;

//Start of AI//
void TankFireState(tankNet::TankBattleCommand* tbc, GlobalData* gd)
{
	//if (sfw::getKey(TANK_FIRE) == true) (tbc)->fireWish = 1;
	(tbc)->fireWish = 1;
}

void TankIdleState(TankBattleCommand* tbc, GlobalData* gd)
{
	(tbc)->msg = TankBattleMessage::NONE;
	(tbc)->tankMove = TankMovementOptions::HALT;
	(tbc)->cannonMove = CannonMovementOptions::HALT;
}

void TankCannonMoveState(TankBattleCommand* tbc, GlobalData* gd)
{
	bool buttonPressed = false;
	if (sfw::getKey(CANN_LEFT) == true) (tbc)->cannonMove = CannonMovementOptions::LEFT, buttonPressed = true;
	if (sfw::getKey(CANN_RIGT) == true) (tbc)->cannonMove = CannonMovementOptions::RIGHT, buttonPressed = true;
	if (buttonPressed == false) (tbc)->cannonMove = CannonMovementOptions::HALT;
}

void TankMoveState(TankBattleCommand* tbc, GlobalData* gd)
{
	Vector2 playerPosition = Vector2((gd)->State->position[0], (gd)->State->position[2]);
	Vector2 playerForward = Vector2((gd)->State->forward[0], (gd)->State->forward[2]);
	float playerForwardAngle = Vector2ToDegree(playerForward);
	//float playerForwardRadian = Vector2ToRadian(playerForward);
	Vector2 playerCannonForward = Vector2((gd)->State->cannonForward[0], (gd)->State->cannonForward[2]);
	float playerCannonForwardAngle = Vector2ToDegree(playerCannonForward);
	//float playerCannonForwardRadian = Vector2ToRadian(playerCannonForward);

	CountTimers(gd);

	if ((gd)->ManualOverride == false)
	{
		if (RestrictedPositionCheck(playerPosition, gd) == false)
		{
			//Wander//
			if ((gd)->State->tacticoolData[0].inSight == 0 && (gd)->State->tacticoolData[1].inSight == 0 && (gd)->State->tacticoolData[2].inSight == 0) (gd)->CurrentAIState = Wander;
			//Pursue//
			else (gd)->CurrentAIState = Pursue;
		}
		//Get Away From Restricted Zone//
		else (gd)->CurrentAIState = Avoid;
	}
}

void WanderState(tankNet::TankBattleCommand* tbc, GlobalData* gd)
{
	Vector2 playerPosition = Vector2((gd)->State->position[0], (gd)->State->position[2]);
	Vector2 playerForward = Vector2((gd)->State->forward[0], (gd)->State->forward[2]);
	float playerForwardAngle = Vector2ToDegree(playerForward);
	//float playerForwardRadian = Vector2ToRadian(playerForward);
	Vector2 playerCannonForward = Vector2((gd)->State->cannonForward[0], (gd)->State->cannonForward[2]);
	float playerCannonForwardAngle = Vector2ToDegree(playerCannonForward);
	//float playerCannonForwardRadian = Vector2ToRadian(playerCannonForward);

	//reset cannon forward rotation
	CannonForwardRotateToTarget(playerForwardAngle, playerCannonForwardAngle, tbc, gd);

	//Check to see if any enemy direction has changed recently
	for (int i = 0; i < 3; ++i)
	{
		if ((gd)->PreviousTargetDirection[i] != Vector2((gd)->State->tacticoolData[i].lastKnownDirection[0], (gd)->State->tacticoolData[i].lastKnownDirection[2]))
		{
			(gd)->TargetLastDirectionUpdateElapsed[i] = 0;
			(gd)->CancelTargetDirectionSearchTimer = 0;
			(gd)->StartCancelTargetDirectionSearch = false;
			(gd)->PreviousTargetDirection[i] = Vector2((gd)->State->tacticoolData[i].lastKnownDirection[0], (gd)->State->tacticoolData[i].lastKnownDirection[2]);
		}
		else
		{
			(gd)->StartCancelTargetDirectionSearch = true;
			(gd)->TargetLastDirectionUpdateElapsed[i] += sfw::getDeltaTime();
		}

		//cout << i + 1 << ": " << (gd)->TargetLastDirectionUpdateElapsed[i] << endl;
	}

	if ((gd)->TargetLastDirectionUpdateElapsed[0] >= (gd)->CancelTargetDirectionSearchDuration && (gd)->TargetLastDirectionUpdateElapsed[1] >= (gd)->CancelTargetDirectionSearchDuration && (gd)->TargetLastDirectionUpdateElapsed[2] >= (gd)->CancelTargetDirectionSearchDuration)
	{
		if ((gd)->ReachedDestination == false)
		{
			float distance = FindDistance(Vector2((gd)->State->position[0], (gd)->State->position[2]), (gd)->TargetDestination);
			//Assign a new random destination
			if (distance > (gd)->ClosestDistanceToDestination) (gd)->ReachedDestination = true;
			//Assign new closest point
			if (distance < (gd)->ClosestDistanceToDestination) (gd)->ClosestDistanceToDestination = distance;

			//Rotate forward
			Vector2 targetDirection = FindDirection(playerPosition, Vector2((gd)->TargetDestination.x, (gd)->TargetDestination.y));
			float targetAngle = Vector2ToDegree(Vector2(targetDirection.x, targetDirection.y));
			if (ForwardRotateToTarget(targetAngle, playerForwardAngle, tbc, gd) == false) (tbc)->tankMove = TankMovementOptions::FWRD;
		}
		//Look for a new random destination
		else
		{
			//For loop for extra speed search
			for (int i = 0; i < 1; ++i)
			{
				(gd)->TargetDestination = RandVec2(-43, 43, -43, 43);
				//If not valid, keep continuing till there is a valid destination
				if (RestrictedPositionCheck((gd)->TargetDestination, gd) == true) i -= 1;
			}
			(gd)->ClosestDistanceToDestination = INT_MAX;
			(gd)->ReachedDestination = false;
		}
	}
	//Attempt to target detected movement
	else
	{
		int targetID = 0;
		float ShortestElapsedTime = INT_MAX;
		for (int i = 0; i < 3; ++i)
		{
			if ((gd)->TargetLastDirectionUpdateElapsed[i] < ShortestElapsedTime)
			{
				targetID = i;
				ShortestElapsedTime = (gd)->TargetLastDirectionUpdateElapsed[i];
			}
		}
		//Incase 2 tanks are near and turret trying to turn to both of them, check which one had the closest position
		Vector2 enemyDirection = Vector2((gd)->State->tacticoolData[targetID].lastKnownDirection[0], (gd)->State->tacticoolData[targetID].lastKnownDirection[2]);
		float enemyDirectionAngle = Vector2ToDegree(enemyDirection);
		CannonForwardRotateToTarget(enemyDirectionAngle, playerCannonForwardAngle, tbc, gd);
		if (ForwardRotateToTarget(enemyDirectionAngle + (gd)->AngleOffset, playerForwardAngle, tbc, gd) == false) (tbc)->tankMove = TankMovementOptions::FWRD;
	}
}

void PursueState(tankNet::TankBattleCommand* tbc, GlobalData* gd)
{
	Vector2 playerPosition = Vector2((gd)->State->position[0], (gd)->State->position[2]);
	Vector2 playerForward = Vector2((gd)->State->forward[0], (gd)->State->forward[2]);
	float playerForwardAngle = Vector2ToDegree(playerForward);
	//float playerForwardRadian = Vector2ToRadian(playerForward);
	Vector2 playerCannonForward = Vector2((gd)->State->cannonForward[0], (gd)->State->cannonForward[2]);
	float playerCannonForwardAngle = Vector2ToDegree(playerCannonForward);
	//float playerCannonForwardRadian = Vector2ToRadian(playerCannonForward);

	float closestEnemyDistance = INT_MAX;
	TankTacticalInfo * closestEnemy = nullptr;
	//Find closest visable target to pursue
	for (int i = 0; i < 3; ++i) //Change value if there are more enemies
	{
		if ((gd)->State->tacticoolData[i].inSight == true)
		{
			float distance = FindDistance(Vector2((gd)->State->position[0], (gd)->State->position[2]), Vector2((gd)->State->tacticoolData[i].lastKnownPosition[0], (gd)->State->tacticoolData[i].lastKnownPosition[2]));
			if (distance < closestEnemyDistance)
			{
				closestEnemy = &(gd)->State->tacticoolData[i];
				closestEnemyDistance = distance;
			}
		}
	}

	if (closestEnemy != nullptr)
	{
		Vector2 enemyPosition = Vector2(closestEnemy->lastKnownPosition[0], closestEnemy->lastKnownPosition[2]);
		Vector2 enemyDirection = Vector2(closestEnemy->lastKnownDirection[0], closestEnemy->lastKnownDirection[2]);
		float enemyDirectionAngle = Vector2ToDegree(enemyDirection);
		Vector2 enemyForward = Vector2(closestEnemy->lastKnownTankForward[0], closestEnemy->lastKnownTankForward[2]);
		float enemyForwardAngle = Vector2ToDegree(enemyForward);
		Vector2 enemyCannonForward = Vector2(closestEnemy->lastKnownCannonForward[0], closestEnemy->lastKnownCannonForward[2]);
		float enemyCannonForwardAngle = Vector2ToDegree(enemyCannonForward);
		float cannonForwardDifference = FindDistance(playerCannonForward, -enemyCannonForward);
		float ForwardDifference = FindDistance(playerForward, -enemyForward);

		CannonForwardRotateToTarget(enemyDirectionAngle, playerCannonForwardAngle, tbc, gd);

		//cout << cannonForwardDifference << endl;

		if (closestEnemyDistance > 22)
		{
			//Predict fire and chase
			if (closestEnemyDistance <= 25 && ForwardDifference <= 0.7f)
			{
				if ((gd)->startBackingup == false)
				{
					//Rotate towards enemy Direction
					if (ForwardRotateToTarget(enemyDirectionAngle + (gd)->AngleOffset, playerForwardAngle, tbc, gd) == false) (tbc)->tankMove = TankMovementOptions::FWRD;
				}
				else (tbc)->tankMove = TankMovementOptions::BACK;
				TankFireState(tbc, gd);
			}
			else
			{
				//Chase
				if ((gd)->startBackingup == false)
				{
					//Rotate towards enemy Direction
					if (ForwardRotateToTarget(enemyDirectionAngle + (gd)->AngleOffset, playerForwardAngle, tbc, gd) == false) (tbc)->tankMove = TankMovementOptions::FWRD;
				}
				else (tbc)->tankMove = TankMovementOptions::BACK;
			}
		}
		//Aggressive fire
		else if (closestEnemyDistance <= 19 && closestEnemyDistance > 5 && cannonForwardDifference > 0.6f)
		{
			//Rotate towards enemy Direction
			ForwardRotateToTarget(enemyDirectionAngle + (gd)->AngleOffset, playerForwardAngle, tbc, gd);
			TankFireState(tbc, gd);
		}
		//Evade fire
		else if (closestEnemyDistance <= 22 && closestEnemyDistance > 5 && cannonForwardDifference <= 0.6f)
		{
			TankFireState(tbc, gd);
			(gd)->startBackingup = true;
			(tbc)->tankMove = TankMovementOptions::BACK;
		}
		//Too close, run
		else
		{
			(gd)->startForcedForward = true;
			(gd)->forceForwardTimer = 0;
			(tbc)->tankMove = TankMovementOptions::FWRD;
		}
	}
}

void AvoidState(tankNet::TankBattleCommand* tbc, GlobalData* gd)
{
	Vector2 playerPosition = Vector2((gd)->State->position[0], (gd)->State->position[2]);
	Vector2 playerForward = Vector2((gd)->State->forward[0], (gd)->State->forward[2]);
	float playerForwardAngle = Vector2ToDegree(playerForward);
	//float playerForwardRadian = Vector2ToRadian(playerForward);
	Vector2 playerCannonForward = Vector2((gd)->State->cannonForward[0], (gd)->State->cannonForward[2]);
	float playerCannonForwardAngle = Vector2ToDegree(playerCannonForward);
	//float playerCannonForwardRadian = Vector2ToRadian(playerCannonForward);

	//Bug: when they see each other they persude and dont pick new path
	//To do: get new valid position to get out of
	(gd)->AngleOffset += 10;
	(gd)->StartAngleOffset = true;
	(gd)->AngleOffsetResetTimer = 0;

	//Fixes bug where if backing up into a retricted, gets stuck
	float closestZone = INT_MAX;
	Vector2 closestZonePosition = Vector2(0, 0);
	for (unsigned int i = 0; i < (gd)->ForbiddenZones.size(); ++i)
	{
		float distance = FindDistance(playerPosition, (gd)->ForbiddenZones[i].Position);
		if (distance < closestZone)
		{
			closestZonePosition = (gd)->ForbiddenZones[i].Position;
			closestZone = distance;
		}
	}

	float zoneDirectionAngle = Vector2ToDegree(FindDirection(playerPosition, closestZonePosition));
	float fixedCurrentAngle = playerForwardAngle - 90;
	if (fixedCurrentAngle < 0) fixedCurrentAngle = fixedCurrentAngle + 360;
	float fixedTargetAngle = zoneDirectionAngle - 90;
	if (fixedTargetAngle < 0) fixedTargetAngle = fixedTargetAngle + 360;
	float shortestArcToTarget = RadianToDegree(shortestArc(DegreeToRadian(fixedTargetAngle), DegreeToRadian(fixedCurrentAngle)));

	if (shortestArcToTarget <= 90 && shortestArcToTarget >= -90) (tbc)->tankMove = TankMovementOptions::BACK;
	else (tbc)->tankMove = TankMovementOptions::FWRD;
	//(gd)->ReachedDestination = true;
}
//End of AI//

void CountTimers(GlobalData* gd)
{
	/*if ((gd)->StartCancelTargetDirectionSearch == true)
	{
		(gd)->CancelTargetDirectionSearchTimer += sfw::getDeltaTime();
		if ((gd)->CancelTargetDirectionSearchTimer >= (gd)->CancelTargetDirectionSearchDuration)
		{
			(gd)->TargetLastDirectionUpdateElapsed[0] = 0;
			(gd)->TargetLastDirectionUpdateElapsed[1] = 0;
			(gd)->TargetLastDirectionUpdateElapsed[2] = 0;
			(gd)->StartCancelTargetDirectionSearch = false;
			(gd)->CancelTargetDirectionSearchTimer = 0;
		}
	}*/

	if ((gd)->StartAngleOffset == true)
	{
		(gd)->AngleOffsetResetTimer += sfw::getDeltaTime();
		if ((gd)->AngleOffsetResetTimer >= (gd)->AngleOffsetResetDuration)
		{
			(gd)->AngleOffset = 0;
			(gd)->StartAngleOffset = false;
			(gd)->AngleOffsetResetTimer = 0;
		}
	}

	if ((gd)->GoToFixedDestination == true)
	{
		(gd)->GoToFixedDestinationTimer += sfw::getDeltaTime();
		if ((gd)->GoToFixedDestinationTimer >= (gd)->GoToFixedDestinationDuration)
		{
			(gd)->GoToFixedDestination = false;
			(gd)->GoToFixedDestinationTimer = 0;
		}
	}

	//Timer for how long the it takes to update the previous health
	if ((gd)->startUpdatingHealth == true)
	{
		(gd)->updateHealthTimer += sfw::getDeltaTime();
		if ((gd)->updateHealthTimer >= (gd)->updateHealthDelay)
		{
			(gd)->previousHealth = (gd)->State->currentHealth;
			(gd)->startUpdatingHealth = false;
			(gd)->updateHealthTimer = 0;
		}
	}

	//Timer for how long the player is forced to go backwards
	if ((gd)->startBackingup == true)
	{
		(gd)->backingupTimer += sfw::getDeltaTime();
		if ((gd)->backingupTimer >= (gd)->backingupDuration)
		{
			(gd)->previousHealth = (gd)->State->currentHealth;
			(gd)->startBackingup = false;
			(gd)->backingupTimer = 0;
		}
	}

	//Timer for how long the player is forced to go forward
	if ((gd)->startForcedForward == true)
	{
		(gd)->forceForwardTimer += sfw::getDeltaTime();
		if ((gd)->forceForwardTimer >= (gd)->forceForwardDuration)
		{
			(gd)->PreviousTankForward = Vector2ToDegree(Vector2((gd)->State->forward[0], (gd)->State->forward[2]));
			(gd)->forceHalfTurnTimer = 0;
			(gd)->startHalfTurn = true;
			(gd)->startForcedForward = false;
			(gd)->forceForwardTimer = 0;
		}
	}

	//Timer for how long the player is forced to turn
	if ((gd)->startHalfTurn == true)
	{
		(gd)->forceHalfTurnTimer += sfw::getDeltaTime();
		if ((gd)->forceHalfTurnTimer >= (gd)->forceHalfTurnDuration)
		{
			(gd)->startHalfTurn = false;
			(gd)->forceHalfTurnTimer = 0;
		}
	}
}

bool ForwardRotateToTarget(float targetAngle, float currentAngle, tankNet::TankBattleCommand* tbc, GlobalData* gd)
{

	float fixedCurrentAngle = currentAngle - 90;
	if (fixedCurrentAngle < 0) fixedCurrentAngle = fixedCurrentAngle + 360;

	float fixedTargetAngle = targetAngle - 90;
	if (fixedTargetAngle < 0) fixedTargetAngle = fixedTargetAngle + 360;
	float shortestArcToTarget = RadianToDegree(shortestArc(DegreeToRadian(fixedTargetAngle), DegreeToRadian(fixedCurrentAngle)));

	//cout << shortestArcToTarget << endl;

	if (shortestArcToTarget < 0 - 17.0f)
	{
		(tbc)->tankMove = TankMovementOptions::LEFT;
		return true;
	}
	else if (shortestArcToTarget > 0 + 17.0f)
	{
		(tbc)->tankMove = TankMovementOptions::RIGHT;
		return true;
	}
	else return false;
}

bool CannonForwardRotateToTarget(float targetAngle, float currentAngle, tankNet::TankBattleCommand* tbc, GlobalData* gd)
{
	float fixedCurrentAngle = currentAngle - 90;
	if (fixedCurrentAngle < 0) fixedCurrentAngle = fixedCurrentAngle + 360;

	float fixedTargetAngle = targetAngle - 90;
	if (fixedTargetAngle < 0) fixedTargetAngle = fixedTargetAngle + 360;
	float shortestArcToTarget = RadianToDegree(shortestArc(DegreeToRadian(fixedTargetAngle), DegreeToRadian(fixedCurrentAngle)));

	if (shortestArcToTarget < 0 - 7.4f)
	{
		(tbc)->cannonMove = CannonMovementOptions::LEFT;
		return true;
	}
	else if (shortestArcToTarget > 0 + 7.4f)
	{
		(tbc)->cannonMove = CannonMovementOptions::RIGHT;
		return true;
	}
	else return false;
}

bool RestrictedPositionCheck(Vector2 position, GlobalData* gd)
{
	for (unsigned int i = 0; i < (gd)->ForbiddenZones.size(); ++i)
	{
		//How far away from the forbidden zone
		if (FindDistance(position, (gd)->ForbiddenZones[i].Position) < 8) return true;
	}
	return false;
}
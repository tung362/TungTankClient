/*
// diagnostic report of current state
stringstream debugStrings;
debugStrings << * globalData.State;
debugStrings << "Tacticool Report:\n";
for (int i = 0; i < globalData.State->tacticoolCount; ++i)
{
debugStrings << globalData.State->tacticoolData[i].playerID << "\n    ";
for (int j = 0; j < 3; ++j)
{
debugStrings << globalData.State->tacticoolData[i].lastKnownPosition[j] << " , ";
}
debugStrings << "\n    ";
for (int j = 0; j < 3; ++j)
{
debugStrings << globalData.State->tacticoolData[i].lastKnownDirection[j] << " , ";
}
debugStrings << "inSight: " << (globalData.State->tacticoolData[i].inSight ? "true" : "false") << "\n";
}

sfw::drawString(globalData.Font, debugStrings.str().c_str(), 0, globalData.WindowHeight, 15, 15);
*/

/*
Notes:
map size is 45x45
have tank search randomly in the area and roate everytime tank reaches destination to find a target if no target, repeat
have tank rotate using degrees
distance to bullet is 19
radius 3
predict fire 25
Cannon forward Difference: 0.6

AI to do:
Autopath using distance of forbidden positions
For targeting Past directions, make player target the direction that updated the most recent (Timer)
-> Incase 2 tanks are near and turret trying to turn to both of them, check which one had the closest position
bug: If player backs up into a wall, it will be stuck
Reflect direction to go depending on how many hazard angles are close
*/







//Run away if got hit
//if ((gd)->previousHealth > (gd)->State->currentHealth)
//{
//	TankFireState(tbc, gd);
//	if ((gd)->State->tacticoolData[0].inSight == 0 && (gd)->State->tacticoolData[1].inSight == 0 && (gd)->State->tacticoolData[2].inSight == 0)
//	{
//		//Check to see if any enemy direction has changed recently
//		for (int i = 0; i < 3; ++i)
//		{
//			if ((gd)->PreviousTargetDirection[i] != Vector2((gd)->State->tacticoolData[i].lastKnownDirection[0], (gd)->State->tacticoolData[i].lastKnownDirection[2]))
//			{
//				(gd)->TargetLastDirectionUpdateElapsed[i] = 0;
//				(gd)->CancelTargetDirectionSearchTimer = 0;
//				(gd)->StartCancelTargetDirectionSearch = false;
//				(gd)->PreviousTargetDirection[i] = Vector2((gd)->State->tacticoolData[i].lastKnownDirection[0], (gd)->State->tacticoolData[i].lastKnownDirection[2]);
//			}
//			else
//			{
//				(gd)->StartCancelTargetDirectionSearch = true;
//				(gd)->TargetLastDirectionUpdateElapsed[i] += sfw::getDeltaTime();
//			}
//
//			//cout << i + 1 << ": " << (gd)->TargetLastDirectionUpdateElapsed[i] << endl;
//		}
//
//		if ((gd)->TargetLastDirectionUpdateElapsed[0] <= (gd)->CancelTargetDirectionSearchDuration && (gd)->TargetLastDirectionUpdateElapsed[1] <= (gd)->CancelTargetDirectionSearchDuration && (gd)->TargetLastDirectionUpdateElapsed[2] <= (gd)->CancelTargetDirectionSearchDuration)
//		{
//			int targetID = 0;
//			float ShortestElapsedTime = INT_MAX;
//			for (int i = 0; i < 3; ++i)
//			{
//				if ((gd)->TargetLastDirectionUpdateElapsed[i] < ShortestElapsedTime)
//				{
//					targetID = i;
//					ShortestElapsedTime = (gd)->TargetLastDirectionUpdateElapsed[i];
//				}
//			}
//			//Incase 2 tanks are near and turret trying to turn to both of them, check which one had the closest position
//			Vector2 enemyDirection = Vector2((gd)->State->tacticoolData[targetID].lastKnownDirection[0], (gd)->State->tacticoolData[targetID].lastKnownDirection[2]);
//			float enemyDirectionAngle = Vector2ToDegree(enemyDirection);
//			CannonForwardRotateToTarget(enemyDirectionAngle, playerCannonForwardAngle, tbc, gd);
//		}
//		(tbc)->tankMove = TankMovementOptions::BACK;
//	}
//	else
//	{
//		//Enemy spoted while running away and reached border (Final Stand)
//		if ((gd)->State->position[0] < -40 || (gd)->State->position[0] > 40 || (gd)->State->position[2] < -40 || (gd)->State->position[2] > 40)
//		{
//			//cout << "Boop" << endl;
//			float closestEnemyDistance = INT_MAX;
//			TankTacticalInfo * closestEnemy = nullptr;
//			//Find closest visable target to pursue
//			for (int i = 0; i < 3; ++i) //Change value if there are more enemies
//			{
//				if ((gd)->State->tacticoolData[i].inSight == true)
//				{
//					float distance = FindDistance(Vector2((gd)->State->position[0], (gd)->State->position[2]), Vector2((gd)->State->tacticoolData[i].lastKnownPosition[0], (gd)->State->tacticoolData[i].lastKnownPosition[2]));
//					if (distance < closestEnemyDistance)
//					{
//						closestEnemy = &(gd)->State->tacticoolData[i];
//						closestEnemyDistance = distance;
//					}
//				}
//			}
//
//			float targetAngle = Vector2ToDegree(Vector2(closestEnemy->lastKnownDirection[0], closestEnemy->lastKnownDirection[2]));
//			float forwardAngle = Vector2ToDegree(Vector2((gd)->State->forward[0], (gd)->State->forward[2]));
//
//			ForwardRotateToTarget(targetAngle, playerForwardAngle, tbc, gd);
//			CannonForwardRotateToTarget(targetAngle, playerCannonForwardAngle, tbc, gd);
//			TankFireState(tbc, gd);
//		}
//		else (tbc)->tankMove = TankMovementOptions::BACK;
//	}
//
//	if ((gd)->startUpdatingHealth == false) (gd)->startUpdatingHealth = true;
//}
//else


//zones[0] = Vector2(-45.0, 15.0);
//zones[1] = Vector2(-35.0, -35.0);
//zones[2] = Vector2(-35.0, -25.0);
//zones[3] = Vector2(-35.0, -15.0);
//zones[4] = Vector2(-35.0, -5.0);
//zones[5] = Vector2(-35.0, 5.0);
//zones[6] = Vector2(-35.0, 15.0);
//zones[7] = Vector2(-35.0, 25.0);
//zones[8] = Vector2(-25.0, -35.0);
//zones[9] = Vector2(-25.0, 5.0);
//zones[10] = Vector2(-5.0, 15.0);
//zones[11] = Vector2(5.0, 5.0);
//zones[12] = Vector2(25.0, -25.0);
//zones[13] = Vector2(25.0, 35.0);
//zones[14] = Vector2(35.0, -35.0);
//zones[15] = Vector2(35.0, 15.0);
//zones[16] = Vector2(35.0, 25.0);
//zones[17] = Vector2(35.0, 35.0);
//zones[18] = Vector2(45.0, -45.0);
//zones[19] = Vector2(45.0, -45.0);
//zones[20] = Vector2(45.0, 15.0);
//zones[21] = Vector2(45.0, 25.0);
//zones[22] = Vector2(-45.0, 5.0);

//float closestZone = INT_MAX;
//Vector2 closestZonePosition = Vector2(0, 0);
//for (unsigned int i = 0; i < (gd)->ForbiddenZones.size(); ++i)
//{
//	float distance = FindDistance(playerPosition, (gd)->ForbiddenZones[i].Position);
//	if (distance < closestZone)
//	{
//		closestZonePosition = (gd)->ForbiddenZones[i].Position;
//		closestZone = distance;
//	}
//}
//
//float zoneDirection = Vector2ToDegree(FindDirection(playerPosition, closestZonePosition));
//float fixedZoneDirection = zoneDirection - 90;
//if (fixedZoneDirection < 0) fixedZoneDirection = fixedZoneDirection + 360;
//cout << fixedZoneDirection << endl;
#pragma once
//#include "GlobalData.h"
//#include "TankBattleNet.h"
//#include "Vector2.h"
#include "ClientFunctions.h"

void TankFireState(tankNet::TankBattleCommand* tbc, GlobalData* gd);
void TankIdleState(tankNet::TankBattleCommand* tbc, GlobalData* gd);
void TankCannonMoveState(tankNet::TankBattleCommand* tbc, GlobalData* gd);
void TankMoveState(tankNet::TankBattleCommand* tbc, GlobalData* gd);

void WanderState(tankNet::TankBattleCommand* tbc, GlobalData* gd);
void PursueState(tankNet::TankBattleCommand* tbc, GlobalData* gd);
void AvoidState(tankNet::TankBattleCommand* tbc, GlobalData* gd);

void CountTimers(GlobalData* gd);
bool ForwardRotateToTarget(float targetAngle, float currentAngle, tankNet::TankBattleCommand* tbc, GlobalData* gd);
bool CannonForwardRotateToTarget(float targetAngle, float currentAngle, tankNet::TankBattleCommand* tbc, GlobalData* gd);
bool RestrictedPositionCheck(Vector2 position, GlobalData* gd);
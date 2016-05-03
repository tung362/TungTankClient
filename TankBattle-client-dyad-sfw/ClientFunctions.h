#pragma once
#include "GlobalData.h"
#include "TankBattleNet.h"
#include "Vector2.h"

//Custom defines//
#define ORANGE 0xf89d28ff;

//Core defines//
#define TANK_FWRD 'W'
#define TANK_BACK 'S'
#define TANK_LEFT 'A'
#define TANK_RIGT 'D'

#define CANN_LEFT 'Q'
#define CANN_RIGT 'E'

#define TANK_FIRE 'F'

#define GAME_QUIT 'L'

#define CLIENT_MAJOR_VERSION 0
#define CLIENT_MINOR_VERSION 1

//Custom functions//
void CreateForbiddenZones(GlobalData* gd);
float RadianToDegree(float radian);
float DegreeToRadian(float degree);
Vector2 RadianToVector2(float radian);
float Vector2ToDegree(Vector2 vec);
float Vector2ToRadian(Vector2 vec);
float shortestArc(float a, float b);
void MathDebug();
int Range(int min, int max);
float Range(float min, float max);
Vector2 RandVec2(float minX, float maxX, float minY, float maxY);
float FindDistance(Vector2 position1, Vector2 position2);
Vector2 FindDirection(Vector2 startPos, Vector2 targetPos);

//Core functions//
bool InputPressed();
int ServerSetup(int argc, char** argv, GlobalData* gd);
void CreateDefaultGlobalData(GlobalData* gd);
void LoadDefaultGlobalDataTexture(GlobalData* gd);
bool CheckTCP(GlobalData* gd);
void DrawTextInfo(GlobalData* gd);
void CreateMinimap(GlobalData* gd);
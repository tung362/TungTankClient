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

void CreateForbiddenZones(GlobalData* gd)
{
	Vector2 zones[8];

	zones[0] = Vector2(-35.0, -35.0);
	zones[1] = Vector2(-35.0, -25.0);
	zones[2] = Vector2(-25.0, -35.0);
	zones[3] = Vector2(-5.0, 15.0);
	zones[4] = Vector2(5.0, 5.0);
	zones[5] = Vector2(25.0, 35.0);
	zones[6] = Vector2(35.0, 25.0);
	zones[7] = Vector2(35.0, 35.0);

	for (int i = 0; i < 8; ++i)
	{
		ForbiddenZone zone = { zones[i] };
		(gd)->ForbiddenZones.push_back(zone);
	}
}

//Custom functions//
float RadianToDegree(float radian)
{
	return (float)(radian * 180 / PI);
}

float DegreeToRadian(float degree)
{
	return (float)(degree * PI / 180);
}

Vector2 RadianToVector2(float radian)
{
	return Vector2(cosf(radian), sinf(radian));
}

float Vector2ToDegree(Vector2 vec)
{
	float radians = (float)atan2(vec.y, vec.x);
	return (float)((radians * 180 / PI));
}

float Vector2ToRadian(Vector2 vec)
{
	return (float)atan2(vec.y, vec.x);
}

float shortestArc(float a, float b)
{
	if (fabs(b - a) < PI)
		return b - a;
	if (b>a)
		return b - a - PI*2.0f;
	return b - a + PI*2.0f;
}

void MathDebug()
{
	Vector2 test(0.7, 0.4);
	cout << "VecToDeg: " << Vector2ToDegree(test) << endl;
	cout << "VecToRad: " << Vector2ToRadian(test) << endl;
	cout << "RadToDeg: " << RadianToDegree(Vector2ToRadian(test)) << endl;
	cout << "DegToRad: " << DegreeToRadian(Vector2ToDegree(test)) << endl;
}

int Range(int min, int max)
{
	return min + (rand() % (max - min + 1));
}

float Range(float min, float max)
{
	return min + (((float)rand()) / (float)RAND_MAX * (max - min));
}

Vector2 RandVec2(float minX, float maxX, float minY, float maxY)
{
	return Vector2(Range(minX, maxX), Range(minY, maxY));
}

float FindDistance(Vector2 position1, Vector2 position2)
{
	return sqrtf((position2.x - position1.x)*(position2.x - position1.x) + (position2.y - position1.y)*(position2.y - position1.y));
}

Vector2 FindDirection(Vector2 startPos, Vector2 targetPos)
{
	Vector2 direction = (targetPos - startPos);
	direction.Normalize();
	return direction;
}

//Core functions//
bool InputPressed()
{
	for (unsigned int i = 0; i < 150; ++i)
	{
		if (sfw::getKey(i))
			return true;
	}

	return false;
}

int ServerSetup(int argc, char** argv, GlobalData* gd)
{
	cout << "Welcome to Tung's client" << endl;
	cout << "Starting Server Setup..." << endl;

	// handle console arguments
	if (argc > 2)
	{
		cout << "Failed Server Setup" << endl;
		std::cout << "Unsupported number of arguments." << std::endl;
		std::cout << "Specify the IP address of the target server. Ex: " << argv[0] << " 127.0.0.1" << std::endl;

		return EXIT_FAILURE;
	}
	else if (argc == 2)
	{
		(gd)->ServerIPAddress = argv[1];
	}

	// initialize networking
	if ((gd)->ServerIPAddress[0] == '\0')
    {
        tankNet::init(); //Remove WHOLE THING TUNG
    }
    else
    {
        tankNet::init((gd)->ServerIPAddress); //Remove Port TUNG
    }

	while (!tankNet::isProvisioned())
	{
		// block further execution until the server responds
		// or until the client gives up on connecting
		tankNet::update();
	}

	cout << "Server Setup Successful" << endl;
	(gd)->MyPlayerID = tankNet::recieve()->playerID;
	return 0;
}

void CreateDefaultGlobalData(GlobalData* gd)
{
	//Settings//
	(gd)->CurrentAIState = Wander;
	(gd)->ActualWindowWidth = 1200;
	(gd)->ActualWindowHeight = 800;
	(gd)->WindowWidth = (gd)->ActualWindowWidth - 400;
	(gd)->WindowHeight = (gd)->ActualWindowHeight;
	(gd)->MyPlayerID = -1;
	(gd)->ServerIPAddress = "10.15.22.73"; //Connor's IP: 10.15.22.125, Jakobob IP: 10.15.22.81 //Dedicated: 10.15.22.31, Torny: 

	(gd)->ManualOverride = false;
	(gd)->ReachedDestination = true;
	(gd)->ClosestDistanceToDestination = INT_MAX;
	(gd)->TargetDestination = Vector2(0, 0);

	(gd)->backingupTimer = 0;
	(gd)->backingupDuration = 0.8f;
	(gd)->startBackingup = false;

	(gd)->forceForwardTimer = 0;
	(gd)->forceForwardDuration = 2.0f;
	(gd)->startForcedForward = false;
	(gd)->forceHalfTurnTimer = 0;
	(gd)->forceHalfTurnDuration = 4.0f;
	(gd)->startHalfTurn = false;
	(gd)->PreviousTankForward = 0;
	(gd)->updateHealthTimer = 0;
	(gd)->updateHealthDelay = 3.0f;
	(gd)->startUpdatingHealth = false;

	(gd)->CurrentEnemySeen = 0;
	(gd)->PreviousEnemySeen = 0;
	(gd)->GoToFixedDestinationTimer = 0;
	(gd)->GoToFixedDestinationDuration = 4.5f;
	(gd)->GoToFixedDestination = true;

	(gd)->CancelTargetDirectionSearchTimer = 0;
	(gd)->CancelTargetDirectionSearchDuration = 5.0f;
	(gd)->StartCancelTargetDirectionSearch = false;
	(gd)->TargetLastDirectionUpdateElapsed[0] = 0;
	(gd)->TargetLastDirectionUpdateElapsed[1] = 0;
	(gd)->TargetLastDirectionUpdateElapsed[2] = 0;
	(gd)->PreviousTargetDirection[0] = Vector2(0, 0);
	(gd)->PreviousTargetDirection[1] = Vector2(0, 0);
	(gd)->PreviousTargetDirection[2] = Vector2(0, 0);

	(gd)->AngleOffsetResetTimer = 0;
	(gd)->AngleOffsetResetDuration = 1.5f;
	(gd)->StartAngleOffset = false;
	(gd)->AngleOffset = 0;

	//Minimap
	(gd)->HALF_WIDTH = ((gd)->WindowWidth / 2);
	(gd)->HALF_HEIGHT = ((gd)->WindowHeight / 2);
}

void LoadDefaultGlobalDataTexture(GlobalData* gd)
{
	//Textures//
	(gd)->Font = sfw::loadTextureMap("./res/fontmap.png", 16, 16);
	(gd)->g_DMAP = sfw::loadTextureMap("./res/MapDiagramCrop.png");
	(gd)->g_DOTS = sfw::loadTextureMap("./res/circArrow.png");
}

bool CheckTCP(GlobalData* gd)
{
	// check TCP streams via dyad
	tankNet::update();
	if (tankNet::isConnected() == false) return false;

	(gd)->State = tankNet::recieve();
	return true;
}

void DrawTextInfo(GlobalData* gd)
{
	std::stringstream info;
	info << "---- Player Info ----" << "\n";
	info << "Player ID: " << (gd)->State->playerID << "\n";
	info << "Health: " << (gd)->State->currentHealth << "\n";
	info << "Position: " << (gd)->State->position[0] << "," << (gd)->State->position[2] << "\n";
	info << "Forward: " << (gd)->State->forward[0] << "," << (gd)->State->forward[1] << "," << (gd)->State->forward[2] << "\n";
	info << "Forward Angle To Target: " << Vector2ToDegree(Vector2((gd)->State->forward[0], (gd)->State->forward[2])) << "\n";
	info << "Cannon Forward: " << (gd)->State->cannonForward[0] << "," << (gd)->State->cannonForward[1] << "," << (gd)->State->cannonForward[2] << "\n";
	info << "Cannon Angle To Target: " << Vector2ToDegree(Vector2((gd)->State->cannonForward[0], (gd)->State->cannonForward[2])) << "\n";
	info << "Can Fire: " << (gd)->State->canFire << "\n";
	info << "Enemies Found: " << (gd)->State->tacticoolCount << "\n";
	info << "\n" << "\n";

	info << "---- Target 1 ----" << "\n";
	info << "ID: " << (gd)->State->tacticoolData[0].playerID << "\n";
	info << "In Sight: " << (gd)->State->tacticoolData[0].inSight << "\n";
	info << "Distance: " << FindDistance(Vector2((gd)->State->position[0], (gd)->State->position[2]), Vector2((gd)->State->tacticoolData[0].lastKnownPosition[0], (gd)->State->tacticoolData[0].lastKnownPosition[2])) << "\n";
	//Accessed by sight, Tells last seen position
	info << "Position: " << (gd)->State->tacticoolData[0].lastKnownPosition[0] << "," << (gd)->State->tacticoolData[0].lastKnownPosition[2] << "\n";
	//Accessed by distance, tells last known direction (Radar)
	info << "Direction: " << (gd)->State->tacticoolData[0].lastKnownDirection[0] << "," << (gd)->State->tacticoolData[0].lastKnownDirection[2] << "\n";
	info << "Direction Angle: " << Vector2ToDegree(Vector2((gd)->State->tacticoolData[0].lastKnownDirection[0], (gd)->State->tacticoolData[0].lastKnownDirection[2])) << "\n";
	info << "\n" << "\n";

	info << "---- Target 2 ----" << "\n";
	info << "ID: " << (gd)->State->tacticoolData[1].playerID << "\n";
	info << "In Sight: " << (gd)->State->tacticoolData[1].inSight << "\n";
	info << "Distance: " << FindDistance(Vector2((gd)->State->position[0], (gd)->State->position[2]), Vector2((gd)->State->tacticoolData[1].lastKnownPosition[0], (gd)->State->tacticoolData[1].lastKnownPosition[2])) << "\n";
	//Accessed by sight, Tells last seen position
	info << "Position: " << (gd)->State->tacticoolData[1].lastKnownPosition[0] << "," << (gd)->State->tacticoolData[1].lastKnownPosition[2] << "\n";
	//Accessed by distance, tells last known direction (Radar)
	info << "Direction: " << (gd)->State->tacticoolData[1].lastKnownDirection[0] << "," << (gd)->State->tacticoolData[1].lastKnownDirection[2] << "\n";
	info << "Direction Angle: " << Vector2ToDegree(Vector2((gd)->State->tacticoolData[1].lastKnownDirection[0], (gd)->State->tacticoolData[1].lastKnownDirection[2])) << "\n";
	info << "\n" << "\n";

	info << "---- Target 3 ----" << "\n";
	info << "ID: " << (gd)->State->tacticoolData[2].playerID << "\n";
	info << "In Sight: " << (gd)->State->tacticoolData[2].inSight << "\n";
	info << "Distance: " << FindDistance(Vector2((gd)->State->position[0], (gd)->State->position[2]), Vector2((gd)->State->tacticoolData[2].lastKnownPosition[0], (gd)->State->tacticoolData[2].lastKnownPosition[2])) << "\n";
	//Accessed by sight, Tells last seen position
	info << "Position: " << (gd)->State->tacticoolData[2].lastKnownPosition[0] << "," << (gd)->State->tacticoolData[2].lastKnownPosition[2] << "\n";
	//Accessed by distance, tells last known direction (Radar)
	info << "Direction: " << (gd)->State->tacticoolData[2].lastKnownDirection[0] << "," << (gd)->State->tacticoolData[2].lastKnownDirection[2] << "\n";
	info << "Direction Angle: " << Vector2ToDegree(Vector2((gd)->State->tacticoolData[2].lastKnownDirection[0], (gd)->State->tacticoolData[2].lastKnownDirection[2])) << "\n";
	info << "\n" << "\n";
	sfw::drawString((gd)->Font, info.str().c_str(), 800, (gd)->WindowHeight, 12, 12);
}

//Minimap
unsigned getColor(int player)
{
	switch (player % 4)
	{
	case 0: return RED;
	case 1: return BLUE;
	case 2: return GREEN;
	case 3: return ORANGE;
	}

	return WHITE;
}

float gth(float val, GlobalData* gd)
{
	return ((val / 100) * (gd)->WindowHeight) + (gd)->HALF_HEIGHT;
}

float gtw(float val, GlobalData* gd)
{
	return ((val / 100) * (gd)->WindowWidth) + (gd)->HALF_WIDTH;
}

void drawArc(float radius, float angleStart, float angleStop, Vector2 pos, unsigned color, GlobalData* gd)
{
	angleStart *= DEG2RAD, angleStop *= DEG2RAD;
	float step = (10 * DEG2RAD);

	while (angleStop < angleStart) { angleStop += 2 * PI; }

	for (float a = angleStart; a < angleStop; a += step)
		if (a + step < angleStop)
			sfw::drawLine(gtw(pos.x + radius * cos(a), gd), gth(pos.y + radius * sin(a), gd), gtw(pos.x + radius * cos(a + step), gd), gth(pos.y + radius * sin(a + step), gd), color);
		else
			sfw::drawLine(gtw(pos.x + radius * cos(a), gd), gth(pos.y + radius * sin(a), gd), gtw(pos.x + radius * cos(angleStop), gd), gth(pos.y + radius * sin(angleStop),gd), color);

}

void drawPlayer(int id, Vector2 pos, Vector2 ang, Vector2 can, GlobalData* gd)
{
	float blah = Vector2ToDegree(can);
	Vector2 relPos = Vector2{ gtw(pos.x, gd), gth(pos.y, gd) };
	Vector2 view1 = Vector2{ std::cos((blah + 45.0f) * DEG2RAD) * 50, std::sin((blah + 45.0f) * DEG2RAD) * 50 };
	Vector2 view2 = Vector2{ std::cos((blah - 45.0f) * DEG2RAD) * 50, std::sin((blah - 45.0f) * DEG2RAD) * 50 };

	//Cannon
	sfw::drawTexture((gd)->g_DOTS, relPos.x, relPos.y, 10.0f, 10.0f, blah - 45);
	//Tank
	sfw::drawTexture((gd)->g_DOTS, relPos.x, relPos.y, 20.0f, 20.0f, (Vector2ToDegree(ang)) - 45, true, 0, getColor(id));
	//Cannon Range
	sfw::drawLine(relPos.x, relPos.y, gtw((can.x * 19) + pos.x, gd), gth((can.y * 19) + pos.y, gd), getColor(id));
	//Shell Blast Radius
	sfw::drawCircle(gtw((can.x * 19) + pos.x, gd), gth((can.y * 19) + pos.y, gd), gtw(5.0f, gd) - (gd)->HALF_WIDTH, 12, getColor(id));
	//Tank Direction
	sfw::drawLine(relPos.x, relPos.y, relPos.x + ang.x * 30, relPos.y + ang.y * 30, getColor(id));
	//Tank LoS
	sfw::drawLine(relPos.x, relPos.y, gtw(view1.x + pos.x, gd), gth(view1.y + pos.y, gd), getColor(id));
	sfw::drawLine(relPos.x, relPos.y, gtw(view2.x + pos.x, gd), gth(view2.y + pos.y,gd), getColor(id));
	drawArc(50, blah - 45.0f, blah + 45.0f, pos, getColor(id), gd);
}

void CreateMinimap(GlobalData* gd)
{
	Vector2 angle = Vector2((gd)->State->forward[0], (gd)->State->forward[2]);
	Vector2 cnAng = Vector2((gd)->State->cannonForward[0], (gd)->State->cannonForward[2]);

	drawPlayer(-5, Vector2((gd)->State->position[0], (gd)->State->position[2]), angle, cnAng, gd);

	for (int a = 0; a < (gd)->State->tacticoolCount; a++)
	{
		if ((gd)->State->tacticoolData[a].isAlive)
		{
			angle = Vector2((gd)->State->tacticoolData[a].lastKnownDirection[0], (gd)->State->tacticoolData[a].lastKnownDirection[2]);
			cnAng = Vector2((gd)->State->tacticoolData[a].lastKnownCannonForward[0], (gd)->State->tacticoolData[a].lastKnownCannonForward[2]);
			drawPlayer((gd)->State->tacticoolData[a].playerID, Vector2((gd)->State->tacticoolData[a].lastKnownPosition[0], (gd)->State->tacticoolData[a].lastKnownPosition[2]), angle, cnAng, gd);
		}
		else
			sfw::drawTexture((gd)->Font, gtw((gd)->State->tacticoolData[a].lastKnownPosition[0], gd), gth((gd)->State->tacticoolData[a].lastKnownPosition[2], gd), 20, 20, 0, true, 88, getColor((gd)->State->tacticoolData[a].playerID));
	}

	sfw::drawTexture((gd)->g_DMAP, (gd)->HALF_WIDTH, (gd)->HALF_HEIGHT, (gd)->WindowWidth, (gd)->WindowHeight);
}
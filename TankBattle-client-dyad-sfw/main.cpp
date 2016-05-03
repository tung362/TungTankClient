#include <iostream>
#include <sstream>
//#include <ctime>

#include "TankBattleNet.h"
#include "sfwdraw.h"
#include "ClientFunctions.h"
#include "GlobalData.h"
#include "Vector2.h"
#include "AI.h"
#undef NONE

using std::stringstream;
using std::cout;
using std::endl;
using namespace tankNet;

int main(int argc, char** argv)
{
	//MathDebug();
	GlobalData globalData;
	//Set default values for global data
	CreateDefaultGlobalData(&globalData);
	//Start client
	sfw::initContext(globalData.ActualWindowWidth, globalData.ActualWindowHeight, "Tung Client");
	sfw::setBackgroundColor(BLACK);
	//Randomize random
	//srand(time(NULL));
	//Load textures
	LoadDefaultGlobalDataTexture(&globalData);

	//Terminate if there is a connection failure
	if (ServerSetup(argc, argv, &globalData) == 1) return EXIT_FAILURE;

	//If a connection was successful...
	if (tankNet::isConnected() && tankNet::isProvisioned())
	{
		//Get first value of health
		globalData.previousHealth = tankNet::recieve()->currentHealth;
		CreateForbiddenZones(&globalData);
		while (sfw::stepContext() && tankNet::isConnected() && tankNet::isProvisioned())
		{
			//Terminate if connection is lost, also updates global data state
			if (CheckTCP(&globalData) == false) break;
			//Draw out debug text info
			DrawTextInfo(&globalData);

			// prepare message
			TankBattleCommand ex;
			//Start of AI//
			TankIdleState(&ex, &globalData);
			TankCannonMoveState(&ex, &globalData);
			TankMoveState(&ex, &globalData);

			if (globalData.ManualOverride == false)
			{
				switch (globalData.CurrentAIState)
				{
				case Wander:
					WanderState(&ex, &globalData);
					break;
				case Pursue:
					PursueState(&ex, &globalData);
					break;
				case Avoid:
					AvoidState(&ex, &globalData);
					break;
				default:
					break;
				}
			}
			//End of AI//

			//Manual Override
			if (sfw::getKey(TANK_FWRD) == true) ex.tankMove = TankMovementOptions::FWRD;
			if (sfw::getKey(TANK_BACK) == true) ex.tankMove = TankMovementOptions::BACK;
			if (sfw::getKey(TANK_LEFT) == true) ex.tankMove = TankMovementOptions::LEFT;
			if (sfw::getKey(TANK_RIGT) == true) ex.tankMove = TankMovementOptions::RIGHT;
			if (sfw::getKey(CANN_LEFT) == true) ex.cannonMove = CannonMovementOptions::LEFT;
			if (sfw::getKey(CANN_RIGT) == true) ex.cannonMove = CannonMovementOptions::RIGHT;
			if (sfw::getKey(TANK_FIRE) == true) ex.fireWish = 1;
			if (sfw::getKey('M') == true) globalData.ManualOverride = true;
			if (sfw::getKey('N') == true) globalData.ManualOverride = false;

			CreateMinimap(&globalData);

			if (InputPressed())
			{
				if (sfw::getKey(GAME_QUIT))
				{
					ex.msg = TankBattleMessage::QUIT;
					break;
				}
			}

			//Begin transmission
			tankNet::send(ex);
		}
	}
	else
	{
		std::cout << "\nFailed to connect to server. Is the server online and not full?";
	}

	tankNet::term();
	sfw::termContext();

	return EXIT_SUCCESS;
}
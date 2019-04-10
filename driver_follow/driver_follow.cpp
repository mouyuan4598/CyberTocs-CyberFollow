/***************************************************************************

				 file : driver_cruise.cpp
	description : user module for CyberFollow

 ***************************************************************************/

 /*
	  WARNING !

	  DO NOT MODIFY CODES BELOW!
 */

#ifdef _WIN32
#include <windows.h>
#endif

#include "driver_follow.h"


static void userDriverGetParam(float LeaderXY[2], float midline[200][2], float yaw, float yawrate, float speed, float acc, float width, int gearbox, float rpm);
static void userDriverSetParam(float* cmdAcc, float* cmdBrake, float* cmdSteer, int* cmdGear);
static int InitFuncPt(int index, void *pt);

// Module Entry Point
extern "C" int driver_follow(tModInfo *modInfo)
{
	memset(modInfo, 0, 10 * sizeof(tModInfo));
	modInfo[0].name = "driver_follow";	// name of the module (short).
	modInfo[0].desc = "user module for CyberFollower";	// Description of the module (can be long).
	modInfo[0].fctInit = InitFuncPt;			// Init function.
	modInfo[0].gfId = 0;
	modInfo[0].index = 0;
	return 0;
}

// Module interface initialization.
static int InitFuncPt(int, void *pt)
{
	tUserItf *itf = (tUserItf *)pt;
	itf->userDriverGetParam = userDriverGetParam;
	itf->userDriverSetParam = userDriverSetParam;
	return 0;
}

/*
	 WARNING!

	 DO NOT MODIFY CODES ABOVE!
*/

/*
	define your variables here.
	following are just examples
*/
static float _midline[200][2];
static float _yaw, _yawrate, _speed, _acc, _width, _rpm;
static int _gearbox;
static float _Leader_X, _Leader_Y;

/*****************self_defined**********************/
static float X2, Y2;
static float leaderSpeed, leaderAcc;
static float ownX, ownY;
static float lastTimeDistance = 0;
static float lastTimeLeaderSpeed = 0;
static float lastTimeLeaderAcc = 0;
static float leaderAccDiff = 0;
static float s;
void updateGear(int *cmdGear);
const int topGear = 6;

static float firstDis = 0, secondDis = 0;
static float leadSpeed = 0, selfSpeed = 0, tempLoc = 0;
/**********************************/

static void userDriverGetParam(float LeaderXY[2], float midline[200][2], float yaw, float yawrate, float speed, float acc, float width, int gearbox, float rpm) {
	/* write your own code here */

	_Leader_X = LeaderXY[0];
	_Leader_Y = LeaderXY[1];
	ownX = midline[0][0];
	ownY = midline[0][1];
	for (int i = 0; i < 200; ++i) _midline[i][0] = midline[i][0], _midline[i][1] = midline[i][1];
	_yaw = yaw;
	_yawrate = yawrate;
	_speed = speed;
	_acc = acc;
	_width = width;
	_rpm = rpm;
	_gearbox = gearbox;
	s = sqrt((ownX - _Leader_X) * (ownX - _Leader_X) + (ownY - _Leader_Y) * (ownY - _Leader_Y));


	/* you can modify the print code here to show what you want */
	//printf("lastTimeDistance: %.3f ", lastTimeDistance);
	//printf("speed %.3f Leader XY(%.3f, %.3f)", _speed, _Leader_X, _Leader_Y);
	//printf("s: %.3f Own XY(%.3f, %.3f)", s, ownX, ownY);
	//printf("speed %.3f ", _speed);
}

static void userDriverSetParam(float* cmdAcc, float* cmdBrake, float* cmdSteer, int* cmdGear) {
	/* write your own code here */

	X2 = _Leader_X * _Leader_X;
	Y2 = _Leader_Y * _Leader_Y;

	leaderSpeed = _speed + (s - lastTimeDistance) * 180;

	firstDis = _Leader_Y;
	leadSpeed = (secondDis - firstDis) / 0.02;




	leaderAcc = (leaderSpeed - lastTimeLeaderSpeed) / 0.02;		// >0  accelerating, <0  decelerating
	lastTimeLeaderSpeed = leaderSpeed;
	printf("self %.2f, lead %.2f dis %.2f acc %.2f brake %.2f leadAcc %.2f\n", _speed, leadSpeed, _Leader_Y, *cmdAcc, *cmdBrake, leaderAcc);

	//leaderAccDiff = leaderAcc - lastTimeLeaderAcc;		//to be used
	//lastTimeLeaderAcc = leaderAcc;						//to be used
	//leaderAccDiff = leaderAcc - lastTimeLeaderAcc;		//to be used

	//if (s <= 20) {											//20 is initial distance
	//	if (leaderSpeed - _speed > 10 && leaderAcc > 0){
	//		*cmdAcc = 0.3;
	//		*cmdBrake = 0;
	//	}
	//	else if (_speed - leaderSpeed > 10 && leaderAcc > 0){
	//		*cmdAcc = 0;
	//		*cmdBrake = 0.3;
	//	}
	//	else {
	//		*cmdAcc = 0;
	//		*cmdBrake = 0.9;
	//	}

	//}
	//else if (s > 20 && s < 25) {
	//	//if (leaderAcc < -30) {
	//	//	*cmdAcc = 0;
	//	//	*cmdBrake = 0.7;
	//	//}
	//	 if (leaderAcc> 30) {
	//		*cmdAcc = 0.8;
	//		*cmdBrake = 0;
	//	}
	//	else if (leaderAcc > 0 && leaderAcc <= 30) {
	//		 *cmdAcc = 0.3;
	//		 *cmdBrake = 0;
	//	 }
	//	else if (_speed - leaderSpeed > 25) {
	//		*cmdAcc = 0;
	//		*cmdBrake = 0.7;
	//	}
	//	else {
	//		*cmdAcc = 0;
	//		*cmdBrake = 0.3;
	//	}
	//}
	//else {														//very far away
	//	if (leaderAcc < -50) {
	//		*cmdAcc = 0;
	//		*cmdBrake = 0.7;
	//	}
	//	else if (leaderAcc > 0 && leaderAcc <= 30) {
	//		*cmdAcc = 0.7;
	//		*cmdBrake = 0;
	//	}
	//	else if (leaderAcc >30) {
	//		*cmdAcc = 1.0;
	//		*cmdBrake = 0;
	//	}
	//	//else if (_speed - leaderSpeed > 10) {
	//	//	*cmdAcc = 0;
	//	//	*cmdBrake = 0.6;
	//	//}
	//	else {
	//		*cmdAcc = 0.3;
	//		*cmdBrake = 0;
	//	}
	//}

	//if (s < lastTimeDistance && s <= 18) {
	//	*cmdAcc = 0;
	//	*cmdBrake = 1.0;
	//}
	//else if(s < lastTimeDistance && s <= 23) {
	//	*cmdAcc = 0;
	//	*cmdBrake = 0.7;
	//}
	//else if (s < lastTimeDistance && s > 23) {
	//	*cmdAcc = 0.2;
	//	*cmdBrake = 0;
	//}
	//else if (s > lastTimeDistance && s <= 18) {
	//	*cmdAcc = 0.2;
	//	*cmdBrake = 0;
	//}
	//else if (s > lastTimeDistance && s <= 23) {
	//	*cmdAcc = 0.4;
	//	*cmdBrake = 0;
	//}
	//else if (s > lastTimeDistance && s > 23) {
	//	*cmdAcc = 1.0;
	//	*cmdBrake = 0;
	//}


	if (_Leader_Y < 13) {
		//if (leadSpeed > 3) {
		//	*cmdAcc = 0;
		//	*cmdBrake = 1.0;
		//}

		if (leaderAcc < -60) {
			*cmdAcc = 0;
			*cmdBrake = 1.0;
		}
		else if (leaderAcc < -30 && leadSpeed > 2.7) {
			*cmdAcc = 0;
			*cmdBrake = 1.0;
		}
		else if (leadSpeed < 0) {
			*cmdAcc = 0.2;
			*cmdBrake = 0;
		}
		else {
			*cmdAcc = 0;
			*cmdBrake = 0.3;
		}
	}

	else if (_Leader_Y < 15) {
		if (leaderAcc < -66) {
			*cmdAcc = 0;
			*cmdBrake = 1.0;
		}
		else if (leadSpeed < 0) {
			*cmdAcc = 0.3;
			*cmdBrake = 0;
		}
		else if (leadSpeed >= 0 && leadSpeed < 1) {
			*cmdAcc = 0.6;
			*cmdBrake = 0;
		}
		//else if (leadSpeed >= 0 && leadSpeed < 1 && abs(_yawrate) >= 0.5) {
		//	*cmdAcc = 0.3;
		//	*cmdBrake = 0;
		//}
		else if (leadSpeed >= 1 && leadSpeed < 2) {
			*cmdAcc = 0.3;
			*cmdBrake = 0;
		}
		//else if (leadSpeed >= 1 && leadSpeed < 2 && abs(_yawrate) >= 0.5) {
		//	*cmdAcc = 0.1;
		//	*cmdBrake = 0;
		//}

		else {
			*cmdAcc = 0;
			*cmdBrake = 0.3;
		}

	}

	else if (_Leader_Y < 20) {
		if (leaderAcc < -66) {
			*cmdAcc = 0;
			*cmdBrake = 1.0;
		}
		else if (leadSpeed > 0 && leadSpeed < 1) {
			*cmdAcc = 0.6;
			*cmdBrake = 0;
		}
		else if (leadSpeed >= 1 && leadSpeed < 2) {
			*cmdAcc = 0.3;
			*cmdBrake = 0;
		}
		else if (leadSpeed < 0) {
			*cmdAcc = 0.8;
			*cmdBrake = 0;
		}
		else {
			*cmdAcc = 0;
			*cmdBrake = 0.3;
		}

	}



	else if (_Leader_Y < 26) {
		if (leaderAcc < -66) {
			*cmdAcc = 0;
			*cmdBrake = 0.8;
		}
		//if (leadSpeed > 6) {
		//	*cmdAcc = 0;
		//	*cmdBrake = 1.0;
		//}
		else if (leadSpeed > 4 && leadSpeed < 7) {
			*cmdAcc = 0.3;
			*cmdBrake = 0;
		}
		//else if (leaderAcc < -70) {
		//	*cmdAcc = 0;
		//	*cmdBrake = 0.8;
		//}
		else if (leadSpeed > 0 && leadSpeed < 1 && abs(_yawrate) < 0.7) {
			*cmdAcc = 0.8;
			*cmdBrake = 0;
		}
		else if (leadSpeed >= 1 && leadSpeed < 2 && abs(_yawrate) >= 0.7) {
			*cmdAcc = 0.2;
			*cmdBrake = 0;
		}
		else if (leadSpeed < 0 && abs(_yawrate) < 0.7) {
			*cmdAcc = 1;
			*cmdBrake = 0;
		}
		else if (leadSpeed < 0 && abs(_yawrate) >= 0.7) {
			*cmdAcc = 0.7;
			*cmdBrake = 0;
		}
		else if (leadSpeed < 0) {
			*cmdAcc = 1;
			*cmdBrake = 0;
		}
		else {
			*cmdAcc = 0;
			*cmdBrake = 0.3;
		}
	}


	else {
		if (leadSpeed > 3) {
			*cmdAcc = 0;
			*cmdBrake = 1.0;
		}
		//else if (leaderAcc < -80) {
		//	*cmdAcc = 0;
		//	*cmdBrake = 0.8;
		//}
		else {
			*cmdAcc = 1.0;
			*cmdBrake = 0;
		}
	}


	lastTimeDistance = s;
	*cmdSteer = (_yaw - 8 * atan2(_Leader_X, _Leader_Y)) / 3.14;

	updateGear(cmdGear);

	secondDis = firstDis;
	/*printf("Leader speed: %.3f ", leaderSpeed);
	printf("cmdAcc: %.2f leaderAcc: %.2f cmdBrake: %.2f ", *cmdAcc, leaderAcc, *cmdBrake);
	printf("s: %.2f\n", s);*/

}

void updateGear(int *cmdGear)
{
	if (_gearbox == 1)
	{
		if (_speed >= 60 && topGear > 1)
		{
			*cmdGear = 2;
		}
		else
		{
			*cmdGear = 1;
		}
	}
	else if (_gearbox == 2)
	{
		if (_speed <= 45)
		{
			*cmdGear = 1;
		}
		else if (_speed >= 105 && topGear > 2)
		{
			*cmdGear = 3;
		}
		else
		{
			*cmdGear = 2;
		}
	}
	else if (_gearbox == 3)
	{
		if (_speed <= 90)
		{
			*cmdGear = 2;
		}
		else if (_speed >= 145 && topGear > 3)
		{
			*cmdGear = 4;
		}
		else
		{
			*cmdGear = 3;
		}
	}
	else if (_gearbox == 4)
	{
		if (_speed <= 131)
		{
			*cmdGear = 3;
		}
		else if (_speed >= 187 && topGear > 4)
		{
			*cmdGear = 5;
		}
		else
		{
			*cmdGear = 4;
		}
	}
	else if (_gearbox == 5)
	{
		if (_speed <= 173)
		{
			*cmdGear = 4;
		}
		else if (_speed >= 234 && topGear > 5)
		{
			*cmdGear = 6;
		}
		else
		{
			*cmdGear = 5;
		}
	}
	else if (_gearbox == 6)
	{
		if (_speed <= 219)
		{
			*cmdGear = 5;
		}
		else
		{
			*cmdGear = 6;
		}
	}
	else
	{
		*cmdGear = 1;
	}
}

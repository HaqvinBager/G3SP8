#pragma once
#include <vector>
#include <SimpleMath.h>

struct SPathPoint {
	Vector3 myPosition;
	Quaternion myRotation;
	float myDuration;
	float myVingetteStrength;
};

//struct SVFXPoint {
//	std::string myVfxPath;
//	float myDelay;
//	int myInstanceID;
//};

struct SEndEventData {
	std::vector<SPathPoint> myEnemyPath;
	//std::vector<SVFXPoint> myVFX;
	int instanceID;
	int myEnemyInstanceID;
	int myPlayerInstanceID;
	float myLockPlayerDuration;
};
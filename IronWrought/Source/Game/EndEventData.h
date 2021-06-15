#pragma once
#include <vector>
#include <SimpleMath.h>

struct SPathPoint {
	Vector3 myPosition;
	Quaternion myRotation;
	float myDuration;
	float myVingetteStrength;
};

struct SEndEventData {
	std::vector<SPathPoint> myEnemyPath;
	int instanceID;
	int myEnemyInstanceID;
	int myPlayerInstanceID;
	float myLockPlayerDuration;
};
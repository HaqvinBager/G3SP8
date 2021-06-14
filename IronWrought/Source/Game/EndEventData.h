#pragma once
#include <vector>
#include <SimpleMath.h>

struct SPathPoint {
	Vector3 myPosition;
	Vector3 rotation;
	float myDuration;
};

struct SEndEventData {
	std::vector<SPathPoint> myEnemyPath;
	int instanceID;
	int myEnemyInstanceID;
	int myPlayerInstanceID;
	float myLockPlayerDuration;
};
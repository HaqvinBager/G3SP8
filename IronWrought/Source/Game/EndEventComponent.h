#pragma once
#include "ResponseBehavior.h"
#include "EndEventData.h"

class CPlayerComponent;

class CEndEventComponent : public IResponseBehavior
{
public:
	CEndEventComponent(CGameObject& aParent, const SEndEventData& aData);
	~CEndEventComponent() override;

	void Awake() override;
	void Start() override;
	void Update() override;
	void FixedUpdate() override {};
	void LateUpdate() override {}
	void OnRespond() override;

	


private:

	const bool IsValidPathIndex(const int aPathIndex);
	void MoveAlongPath(const SPathPoint& aPoint);
	void UpdateAnimation(const SPathPoint& aPoint);
	void UpdateVignette(const SPathPoint& aPoint);
	void UpdatePathIndex(const SPathPoint& point);
	void StartPostEvent();
	void OnEndEventComplete();

	const float Lerp(const float a, const float b, const float t) const
	{
		return std::clamp((1.0f - t)* a + b * t, a, b);
	}

	inline float SmoothStep(const float a, const float b, const float t) const
	{
		float x = std::clamp((t - a) / (b - a), 0.0f, 1.0f);
		return x * x * (3.0f - 2.0f * x);
	}

	inline float InverseLerp(const float a, const float b, const float v) const
	{
		return std::clamp((v - a) / (b - a), 0.0f, 1.0f);
	}



	SEndEventData myData;
	Quaternion myLastRotation;
	Vector3 myLastPos;
	float myLastVignetteStrength;
	int myLastAnimationIndex;
	int myPathIndex;
	float myTime;
	float myVignetteTime;
	bool myEventHasCompleted;

	CGameObject* myPlayer;
	CGameObject* myEnemy;

	//Imgui Hierarchy Data

public:
	const float VingetteNormalizedBlend() const { return myNormalizedBlend; }
	const float ActualVingetteStrength() const { return myActualVingetteStrength; }
	const SPathPoint& CurrentPoint() const { return myPathIndex == -1 ? myData.myEnemyPath[0] : myData.myEnemyPath[myPathIndex]; }

private:
	float myNormalizedBlend = 0.0f;
	float myActualVingetteStrength = 0.0f;
	float myHalfTime;
	float myVignetteStrength;
	bool myHasStartedPostEvent;
};


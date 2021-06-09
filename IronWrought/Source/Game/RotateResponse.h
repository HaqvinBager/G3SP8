#pragma once
#include "ResponseBehavior.h"
#include "PuzzleSetting.h"







class CRotateResponse : public IResponseBehavior
{
public:
	//struct SSettings
	//{
	//	Vector3 myStartRotation;
	//	Vector3 myEndRotation;
	//	float myDuration;
	//	float myDelay;
	//};

	CRotateResponse(CGameObject& aParent, const SSettings<Quaternion>& someSettings);
	~CRotateResponse() override { }
	void Update() override;
	void OnRespond() override;

private:
	SSettings<Quaternion> mySettings;
	float myTime;
	//Quaternion myStart;
	//Quaternion myEnd;
};


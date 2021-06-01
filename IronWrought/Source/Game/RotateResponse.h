#pragma once
#include "ResponseBehavior.h"

class CRotateResponse : public IResponseBehavior
{
public:
	struct SSettings
	{
		Vector3 myStartRotation;
		Vector3 myEndRotation;
		float myDuration;
		float myDelay;
	};

	CRotateResponse(CGameObject& aParent, const SSettings& someSettings);
	~CRotateResponse() override { }
	void Update() override;
	void OnRespond() override;

private:
	SSettings mySettings;
	float myTime;
	Quaternion myStartRotation;
};


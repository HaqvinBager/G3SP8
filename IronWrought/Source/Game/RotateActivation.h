#pragma once
#include "ActivationBehavior.h"

class CRotateActivation : public IActivationBehavior
{
public:
	struct SSettings
	{
		Vector3 myStartRotation;
		Vector3 myEndRotation;
		float myDuration;
	};

	CRotateActivation(CGameObject& aParent, const SSettings& someSettings);
	~CRotateActivation() override;
	void Update() override;

private:
	SSettings mySettings;
	float myTime;
	Quaternion myStart;
	Quaternion myEnd;
};
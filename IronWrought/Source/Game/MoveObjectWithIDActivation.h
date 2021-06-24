#pragma once
#include "ActivationBehavior.h"
class CMoveObjectWithIDActivation : public IActivationBehavior
{
public:
	struct SSettings
	{
		Vector3 myStartPosition;
		Vector3 myEndPosition;
		float myDuration;
		float myDelay;
		int myGOIDToMove;
	};

	CMoveObjectWithIDActivation(CGameObject& aParent, const SSettings& someSettings);
	~CMoveObjectWithIDActivation() override;
	void Update() override;

private:
	SSettings mySettings;
	float myTime;
	bool myHasBeenDelayed;
};


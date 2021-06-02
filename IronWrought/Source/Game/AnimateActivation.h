#pragma once
#include "ActivationBehavior.h"

class CAnimateActivation : public IActivationBehavior
{
public:
	struct SSettings
	{
		Vector3 myStartPosition;
		Vector3 myEndPosition;

		Vector3 myStartRotation;
		Vector3 myEndRotation;
		
		float myDuration;
	};

	CAnimateActivation(CGameObject& aParent, const SSettings& someSettings);
	~CAnimateActivation() override;
	void OnActivation() override;
	void Update() override;
private:
	SSettings mySettings;
	bool myIsInteracted;
	float myTime;
};
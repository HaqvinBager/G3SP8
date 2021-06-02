#pragma once
#include "ActivationBehavior.h"
class CMoveActivation : public IActivationBehavior
{
public:
	struct SSettings
	{
		Vector3 myStartPosition;
		Vector3 myEndPosition;

		float myDuration;
	};

	CMoveActivation(CGameObject& aParent, const SSettings& someSettings);
	~CMoveActivation() override;
	void OnActivation() override;
	void Update() override;
private:
	SSettings mySettings;
	bool myIsInteracted;
	float myTime;
};


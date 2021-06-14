#pragma once
#include "ActivationBehavior.h"
#include "PuzzleSetting.h"

class CRotateActivation : public IActivationBehavior
{
public:
	CRotateActivation(CGameObject& aParent, const SSettings<Quaternion>& someSettings);
	~CRotateActivation() override;
	void Update() override;

private:
	SSettings<Quaternion> mySettings;
	float myTime;
};
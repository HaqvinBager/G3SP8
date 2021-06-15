#pragma once
#include "ActivationBehavior.h"

class CVFXSystemComponent;

class CPlayVFXActivation : public IActivationBehavior
{
public:
	struct SSettings
	{
		float myDuration;
	};

	CPlayVFXActivation(CGameObject& aParent, SSettings someSettings);
	~CPlayVFXActivation() override;
	void Start() override;
	void OnActivation() override;
	void Update() override;

private:
	SSettings mySettings;
	CVFXSystemComponent* myVFXSystemComponent;
	float myTime;
	bool myHasVFX;
};


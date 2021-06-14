#pragma once
#include "ResponseBehavior.h"

class CVFXSystemComponent;

class CPlayVFXResponse : public IResponseBehavior
{
public:
	struct SSettings
	{
		float myDuration;
		float myDelay;
	};

	CPlayVFXResponse(CGameObject& aParent, const SSettings& someSettings);
	~CPlayVFXResponse() override { }
	void Start() override;
	void Update() override;
	void OnRespond() override;

private:
	SSettings mySettings;
	CVFXSystemComponent* myVFXSystemComponent;
	float myTime;
};


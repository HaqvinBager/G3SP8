#pragma once
#include "ResponseBehavior.h"

class CMoveResponse : public IResponseBehavior
{
public:
	struct SSettings
	{
		Vector3 myStartPosition;
		Vector3 myEndPosition;
		float myDuration;
	};
	
	CMoveResponse(CGameObject& aParent, const SSettings& someSettings);
	~CMoveResponse();
	void Update() override;
	void OnRespond() override;

private:
	SSettings mySettings;
	float myTime;
};
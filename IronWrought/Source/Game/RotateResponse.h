#pragma once
#include "ResponseBehavior.h"
#include "PuzzleSetting.h"

class CRotateResponse : public IResponseBehavior
{
public:

	CRotateResponse(CGameObject& aParent, const SSettings<Quaternion>& someSettings);
	~CRotateResponse() override { }
	void Update() override;
	void OnRespond() override;

private:
	SSettings<Quaternion> mySettings;
	float myTime;
};


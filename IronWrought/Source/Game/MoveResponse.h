#pragma once
#include "ResponseBehavior.h"
#include "PuzzleSetting.h"

class CMoveResponse : public IResponseBehavior
{
public:	
	CMoveResponse(CGameObject& aParent, const SSettings<Vector3>& someSettings);
	~CMoveResponse() override { }
	void Update() override;
	void OnRespond() override;

private:
	SSettings<Vector3> mySettings;
	float myTime;
};
#pragma once
#include "ResponseBehavior.h"
#include "PuzzleSetting.h"

class CMoveObjectWithIDResponse : public IResponseBehavior
{
public:
	CMoveObjectWithIDResponse(CGameObject& aParent, const SSettings<Vector3>& someSettings, const int& aGOID);
	~CMoveObjectWithIDResponse() override { }
	void Update() override;
	void OnRespond() override;

private:
	SSettings<Vector3> mySettings;
	int myGOIDToMove;
	float myTime;
};

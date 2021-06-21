#pragma once
#include "ResponseBehavior.h"
class CAddForceResponse :public IResponseBehavior
{
public:
	struct SSettings {
		Vector3 myDirection;
		float myForce;
	};

	CAddForceResponse(CGameObject& aParent, const SSettings& someSettings);
	~CAddForceResponse() override;
	void OnRespond() override;

	void FixedUpdate() override;


private:
	SSettings mySettings;
};


#pragma once
#include "ResponseBehavior.h"

class CPrintResponse : public IResponseBehavior
{
public:
	struct SSettings
	{
		std::string myData;
		float myDelay;
	};
	CPrintResponse(CGameObject& aParent, const SSettings& someSettings);
	~CPrintResponse() override { }
	void Update() override;
	void OnRespond() override;
private:
	SSettings mySettings;
};


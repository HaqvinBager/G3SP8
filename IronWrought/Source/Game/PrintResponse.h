#pragma once
#include "ResponseBehavior.h"

class CPrintResponse : public IResponseBehavior
{
public:
	CPrintResponse(CGameObject& aParent);
	~CPrintResponse() override { }
	void Update() override;
	void OnRespond() override;

};


#include "stdafx.h"
#include "PrintResponse.h"

CPrintResponse::CPrintResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
{
	Enabled(false);
}

void CPrintResponse::Update()
{
	std::cout << __FUNCTION__ << mySettings.myData << "\n";
	Enabled(false);
}

void CPrintResponse::OnRespond()
{
	Enabled(true);
}
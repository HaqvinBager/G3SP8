#include "stdafx.h"
#include "PrintResponse.h"

CPrintResponse::CPrintResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
{
	Enabled(false);
}

void CPrintResponse::Update()
{
	std::cout << mySettings.myData << "\n";
	Enabled(false);
}

void CPrintResponse::OnRespond()
{
	Enabled(true);
}
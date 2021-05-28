#include "stdafx.h"
#include "PrintResponse.h"

CPrintResponse::CPrintResponse(CGameObject& aParent)
	: IResponseBehavior(aParent)
{
	Enabled(false);
}

void CPrintResponse::Update()
{
	std::cout << ResponseNotify() << "has triggered!\n";
	Enabled(false);
}

void CPrintResponse::OnRespond()
{
	Enabled(true);
}
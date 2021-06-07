#include "stdafx.h"
#include "PrintResponse.h"

CPrintResponse::CPrintResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
{
	HasBeenActivated(false);
}

void CPrintResponse::Update()
{
	if (!HasBeenActivated())
		return;

	std::cout << __FUNCTION__ << mySettings.myData << "\n";
	HasBeenActivated(false);
}

void CPrintResponse::OnRespond()
{
	HasBeenActivated(true);
}
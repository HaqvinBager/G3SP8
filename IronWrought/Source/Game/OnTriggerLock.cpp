#include "stdafx.h"
#include "OnTriggerLock.h"

COnTriggerLock::COnTriggerLock(CGameObject& aParent, const SSettings someSettings) : CLockBehavior(aParent, someSettings)
{
}

COnTriggerLock::~COnTriggerLock()
{
}

void COnTriggerLock::ActivateEvent()
{
	if (Enabled())
	{
		RunEvent();
	}
	//do some check
	//then
}


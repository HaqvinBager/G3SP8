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
	//do some check
	//then
	RunEvent();
}


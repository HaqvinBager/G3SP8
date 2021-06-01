#include "stdafx.h"
#include "OnLookLock.h"

COnLookLock::COnLookLock(CGameObject& aParent, const SSettings someSettings)
	: CLockBehavior(aParent, someSettings)
{
}

COnLookLock::~COnLookLock()
{
}

void COnLookLock::ActivateEvent()
{
	RunEvent();
}

#include "stdafx.h"
#include "LeftClickDownLock.h"

CLeftClickDownLock::CLeftClickDownLock(CGameObject& aParent, const SSettings someSettings) : CLockBehavior(aParent, someSettings)
{
}

CLeftClickDownLock::~CLeftClickDownLock()
{
}

void CLeftClickDownLock::ActivateEvent()
{
	//do some check
	//then
	RunEvent();
}
#include "stdafx.h"
#include "InstantActivationLock.h"

CInstantActivationLock::CInstantActivationLock(CGameObject& aParent, const SSettings someSettings) : CLockBehavior(aParent, someSettings)
{
}

CInstantActivationLock::~CInstantActivationLock()
{
}

void CInstantActivationLock::Update()
{
	if (!myHasTriggered)
	{
		if (AmountOfKeys() >= MaxAmountOfKeys())
		{
			RunEvent();
		}
	}
}

void CInstantActivationLock::ActivateEvent()
{
}

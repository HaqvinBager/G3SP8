#include "stdafx.h"
#include "InstantActivationLock.h"

CInstantActivationLock::CInstantActivationLock(CGameObject& aParent, const SSettings someSettings)
	: CLockBehavior(aParent, someSettings)
{
}

CInstantActivationLock::~CInstantActivationLock()
{
}
//
//void CInstantActivationLock::Update()
//{
//	//if (myKeyControlsThisLock)
//	//	return;
//
//	//if (!myHasTriggered)
//	//{
//	//	if (AmountOfKeys() >= MaxAmountOfKeys())
//	//	{
//	//		ActivateEvent();
//	//	}
//	//}
//}

void CInstantActivationLock::OnKeyActivated(CKeyBehavior* /*aKey*/)
{
	myAmountOfKeys++;
	if (myAmountOfKeys == MaxAmountOfKeys())
	{
		ActivateEvent();
	}
}

void CInstantActivationLock::ActivateEvent()
{
	std::cout << __FUNCTION__ << " -- Instant Lock Activation Triggered" << std::endl;
	if (myAmountOfKeys == myAmountOfKeys)
	{
		if (Enabled())
		{
			RunEvent();
			myHasTriggered = true;
			Enabled(false);
		}
	}
}

void CInstantActivationLock::AddKey(CKeyBehavior* /*aKey*/)
{
	myMaxAmountOfKeys++;
}

void CInstantActivationLock::RemoveKey(CKeyBehavior* /*aKey*/)
{
	myMaxAmountOfKeys--;
}
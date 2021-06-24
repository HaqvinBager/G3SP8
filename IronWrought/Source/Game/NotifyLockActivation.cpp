#include "stdafx.h"
#include "NotifyLockActivation.h"
#include "Scene.h"
#include "InstantActivationLock.h"

CNotifyLockActivation::CNotifyLockActivation(CGameObject& aParent, const SSettings& someSettings)
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myLockToNotify(nullptr)
{
}

CNotifyLockActivation::~CNotifyLockActivation()
{
}

void CNotifyLockActivation::OnEnable()
{
	CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myLockToNotifyInstanceID);
	if (lockGameObject != nullptr)
	{
		CInstantActivationLock* lockToNotify = nullptr;
		if (lockGameObject->TryGetComponentAny(&lockToNotify))
		{
			lockToNotify->AddKey(nullptr);
			myLockToNotify = lockToNotify;
			IActivationBehavior::OnEnable();
		}
	}
}

void CNotifyLockActivation::OnDisable()
{
	CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myLockToNotifyInstanceID);
	if (lockGameObject != nullptr)
	{
		CInstantActivationLock* lockToNotify = nullptr;
		if (lockGameObject->TryGetComponentAny(&lockToNotify))
		{
			lockToNotify->RemoveKey(nullptr);
			myLockToNotify = nullptr;
			IActivationBehavior::OnDisable();
		}
	}
}


void CNotifyLockActivation::OnActivation()
{
	std::cout << __FUNCTION__ << " Activated " << GameObject().Name() << std::endl;
	if (myLockToNotify != nullptr)
	{
		myLockToNotify->OnKeyActivated(nullptr);
	}
}

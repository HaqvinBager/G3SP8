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
	if (Enabled())
	{
		if (mySettings.myOnNotifyName.find("OpenBunkerDoor") != std::string::npos)
		{
			CMainSingleton::PostMaster().Send({ EMessageType::FoundKey, &GameObject() });
		}

		RunEvent();
	}
}

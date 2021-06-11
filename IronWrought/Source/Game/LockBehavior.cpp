#include "stdafx.h"
#include "LockBehavior.h"

CLockBehavior::CLockBehavior(CGameObject& aParent, const SSettings someSettings) :
	CBehavior(aParent),
	mySettings(someSettings),
	myMaxAmountOfKeys(0),
	myAmountOfKeys(0),
	myHasTriggered(false)
{
	//CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify, this);
	//CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify, this);
}

CLockBehavior::~CLockBehavior()
{
	
}

void CLockBehavior::OnEnable()
{
	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify, this);
	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify, this);
}

void CLockBehavior::OnDisable()
{
	CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyCreateNotify, this);
	CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyInteractNotify, this);
	myAmountOfKeys = 0;
}

void CLockBehavior::RunEvent()
{
	if (!myHasTriggered)
	{
		if (myAmountOfKeys >= myMaxAmountOfKeys)
		{
			std::cout << __FUNCTION__ << "----< \tLock Activated \t" << GameObject().Name() << std::endl;
			myHasTriggered = true;
			CMainSingleton::PostMaster().Send({ mySettings.myOnNotifyName.c_str(), mySettings.myOnNotify });
		}
	}
}

void CLockBehavior::RunEventEditor()
{
	if (!myHasTriggered)
	{
		myHasTriggered = true;
		CMainSingleton::PostMaster().Send({ mySettings.myOnNotifyName.c_str(), mySettings.myOnNotify });
	}
}


void CLockBehavior::Receive(const SIDMessage& aMessage)
{
	if (mySettings.myOnKeyCreateNotify == aMessage.myMessageID)
	{
		++myMaxAmountOfKeys;
		std::cout << __FUNCTION__ << "----| \t" << GameObject().Name() << " Keys: " << myMaxAmountOfKeys << "   Message: " << aMessage.myName << std::endl;
	}
	else
	{
		++myAmountOfKeys;
		std::cout << __FUNCTION__ << "----> \t" << GameObject().Name() << " Key Pickup: " << myAmountOfKeys << std::endl;
	}
}



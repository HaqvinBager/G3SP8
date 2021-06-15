#include "stdafx.h"
#include "LockBehavior.h"

CLockBehavior::CLockBehavior(CGameObject& aParent, const SSettings someSettings) :
	CBehavior(aParent),
	mySettings(someSettings),
	myMaxAmountOfKeys(0),
	myAmountOfKeys(0),
	myHasTriggered(false),
	myHasSubscribed(false)
{
	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify, this);
	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify, this);
	myHasSubscribed = true;
}

CLockBehavior::~CLockBehavior()
{

}

void CLockBehavior::OnEnable()
{
	if (!myHasSubscribed)
	{
		CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify, this);
		CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify, this);
		myHasSubscribed = true;
	}
}

void CLockBehavior::OnDisable()
{
	CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyCreateNotify, this);
	CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyInteractNotify, this);
	myAmountOfKeys = 0;
	myHasSubscribed = false;
}

void CLockBehavior::RunEvent()
{
	if (!myHasTriggered)
	{
		if (myAmountOfKeys >= myMaxAmountOfKeys)
		{
			CMainSingleton::PostMaster().SendLate({ EMessageType::FoundKey, &this->GameObject() });
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
		
		if (myMaxAmountOfKeys == 3)
		{
			if (myAmountOfKeys < myMaxAmountOfKeys)
			{
				int soundIndex = 29;
				CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &soundIndex });
			}
			else if (myAmountOfKeys >= myMaxAmountOfKeys)
			{
				int soundIndex = 30;
				CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &soundIndex });
			}
		}

		std::cout << __FUNCTION__ << "----> \t" << GameObject().Name() << " Key Pickup: " << myAmountOfKeys << std::endl;
	}
}



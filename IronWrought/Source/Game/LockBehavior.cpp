#include "stdafx.h"
#include "LockBehavior.h"
#include "ListenerBehavior.h"
#include "KeyBehavior.h"
#include "AudioActivation.h"
#include "AudioManager.h"

CLockBehavior::CLockBehavior(CGameObject& aParent, const SSettings someSettings) :
	CBehavior(aParent),
	mySettings(someSettings),
	myMaxAmountOfKeys(0),
	myAmountOfKeys(0),
	myHasTriggered(false),
	myHasSubscribed(false)
{
	//CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify, this);
	//CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify, this);
	//myHasSubscribed = true;
}

void CLockBehavior::Register(CListenerBehavior* aListener)
{
	auto it = std::find(myListeners.begin(), myListeners.end(), aListener);
	if (it == myListeners.end())
	{
		myListeners.push_back(aListener);
	}
}

void CLockBehavior::Unregister(CListenerBehavior* aListener)
{
	auto it = std::find(myListeners.begin(), myListeners.end(), aListener);
	if (it != myListeners.end())
	{
		myListeners.erase(it);
	}
}

void CLockBehavior::AddKey(CKeyBehavior* aKey)
{
	auto it = std::find(myKeys.begin(), myKeys.end(), aKey);
	if (it == myKeys.end())
	{
		myKeys.push_back(aKey);
		myMaxAmountOfKeys = static_cast<int>(myKeys.size());
	}
}

void CLockBehavior::RemoveKey(CKeyBehavior* aKey)
{
	auto it = std::find(myKeys.begin(), myKeys.end(), aKey);
	if (it != myKeys.end())
	{
		myKeys.erase(it);
		//myMaxAmountOfKeys = static_cast<int>(myKeys.size());
	}
}

void CLockBehavior::OnKeyActivated(CKeyBehavior* aKey)
{
	myAmountOfKeys++;
	std::cout << __FUNCTION__ << " Keys: " << myAmountOfKeys << std::endl;

	if (myMaxAmountOfKeys == 3)
	{
		CAudioActivation* audioActivation = nullptr;
		if (aKey->GameObject().TryGetComponent(&audioActivation))
		{
			if (myAmountOfKeys < myMaxAmountOfKeys)
			{
				int soundIndex = 29;
				CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &soundIndex });
				CMainSingleton::PostMaster().Send({ EMessageType::FoundKey, &aKey->GameObject() });
			}
			else if (myAmountOfKeys >= myMaxAmountOfKeys)
			{
				int soundIndex = 30;
				CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &soundIndex });
				CMainSingleton::PostMaster().Send({ EMessageType::FoundKey, &aKey->GameObject() });
			}
		}
	}
}

CLockBehavior::~CLockBehavior()
{

}

void CLockBehavior::OnEnable()
{	
	//if (!myHasSubscribed)
	//{
	//	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify, this);
	//	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify, this);
	//	myHasSubscribed = true;
	//}
}

void CLockBehavior::OnDisable()
{

	//CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyCreateNotify, this);
	//CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyInteractNotify, this);
	myAmountOfKeys = 0;
	myHasSubscribed = false;
}

void CLockBehavior::RunEvent()
{
	if (!myHasTriggered)
	{
		if (myAmountOfKeys >= myMaxAmountOfKeys)
		{
			for (auto& listener : myListeners)
			{
				listener->TriggerResponses();
			}

			std::cout << __FUNCTION__ << "----< \tLock Activated \t" << GameObject().Name() << std::endl;
			myHasTriggered = true;
			//CMainSingleton::PostMaster().Send({ mySettings.myOnNotifyName.c_str(), mySettings.myOnNotify });
		}
	}
}

void CLockBehavior::RunEventEditor()
{
	if (!myHasTriggered)
	{
		for (auto& listener : myListeners)
		{
			listener->TriggerResponses();
		}
		myHasTriggered = true;
		//CMainSingleton::PostMaster().Send({ mySettings.myOnNotifyName.c_str(), mySettings.myOnNotify });
	}
}


void CLockBehavior::Receive(const SIDMessage& /*aMessage*/)
{
	//if (mySettings.myOnKeyCreateNotify == aMessage.myMessageID)
	//{
	//	++myMaxAmountOfKeys;
	//	std::cout << __FUNCTION__ << "----| \t" << GameObject().Name() << " Keys: " << myMaxAmountOfKeys << "   Message: " << aMessage.myName << std::endl;
	//}
	//else
	//{
	//	++myAmountOfKeys;
	//	
	//	if (myMaxAmountOfKeys == 3)
	//	{
	//		if (myAmountOfKeys < myMaxAmountOfKeys)
	//		{
	//			int soundIndex = 29;
	//			CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &soundIndex });
	//		}
	//		else if (myAmountOfKeys >= myMaxAmountOfKeys)
	//		{
	//			int soundIndex = 30;
	//			CMainSingleton::PostMaster().Send({ EMessageType::PlaySFX, &soundIndex });
	//		}
	//	}

	//	std::cout << __FUNCTION__ << "----> \t" << GameObject().Name() << " Key Pickup: " << myAmountOfKeys << std::endl;
	//}
}



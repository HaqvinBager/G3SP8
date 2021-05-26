#include "stdafx.h"
#include "LockComponent.h"

CLockComponent::CLockComponent(CGameObject& aParent, std::string aCreateListenStringMessage, std::string aDestroyListenStringMessage, std::string aSendStringMessage, void* someData) :
	CBehaviour(aParent),
	myCreateListenStringMessage(aCreateListenStringMessage),
	myDestroyListenStringMessage(aDestroyListenStringMessage),
	mySendStringMessage(aSendStringMessage),
	myMaxAmountOfKeys(0),
	myAmountOfKeys(0),
	myHasTriggered(false),
	myData(someData)
{
	CMainSingleton::PostMaster().Subscribe(myCreateListenStringMessage.c_str(), this);
	CMainSingleton::PostMaster().Subscribe(myDestroyListenStringMessage.c_str(), this);
}

CLockComponent::~CLockComponent()
{
}

void CLockComponent::Destroy()
{
}

void CLockComponent::Awake()
{
}

void CLockComponent::Start()
{
}

void CLockComponent::Update()
{
}

void CLockComponent::OnEnable()
{
}

void CLockComponent::OnDisable()
{
}

void CLockComponent::RunEvent()
{
	if (!myHasTriggered)
	{
		if (myMaxAmountOfKeys <= myAmountOfKeys)
		{
			myHasTriggered = true;
			CMainSingleton::PostMaster().Send({mySendStringMessage.c_str(), myData});
		}
	}
}

void CLockComponent::Receive(const SStringMessage& aMessage)
{
	if (aMessage.myMessageType == myCreateListenStringMessage.c_str())
		++myMaxAmountOfKeys;
	else
		++myAmountOfKeys;
}
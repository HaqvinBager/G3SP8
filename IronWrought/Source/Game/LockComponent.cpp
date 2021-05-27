#include "stdafx.h"
#include "LockComponent.h"

CLockComponent::CLockComponent(CGameObject& aParent, std::string aCreateReceiveMessage, std::string aDestroyReceiveMessage, std::string aSendMessage, void* someData) :
	CBehavior(aParent),
	myCreateReceiveMessage(aCreateReceiveMessage),
	myDestroyReceiveMessage(aDestroyReceiveMessage),
	mySendMessage(aSendMessage),
	myMaxAmountOfKeys(0),
	myAmountOfKeys(0),
	myHasTriggered(false),
	myData(someData)
{
	CMainSingleton::PostMaster().Subscribe(myCreateReceiveMessage.c_str(), this);
	CMainSingleton::PostMaster().Subscribe(myDestroyReceiveMessage.c_str(), this);
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
			CMainSingleton::PostMaster().Send({mySendMessage.c_str(), myData});
		}
	}
}

void CLockComponent::Receive(const SStringMessage& aMessage)
{
	if (aMessage.myMessageType == myCreateReceiveMessage.c_str())
		++myMaxAmountOfKeys;
	else
		++myAmountOfKeys;
}
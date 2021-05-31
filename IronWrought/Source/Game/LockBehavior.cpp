#include "stdafx.h"
#include "LockBehavior.h"

CLockBehavior::CLockBehavior(CGameObject& aParent, const SSettings someSettings) :
	CBehavior(aParent),
	mySettings(someSettings),
	myMaxAmountOfKeys(0),
	myAmountOfKeys(0),
	myHasTriggered(false)
{
	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyCreateNotify.c_str(), this);
	CMainSingleton::PostMaster().Subscribe(mySettings.myOnKeyInteractNotify.c_str(), this);
}

CLockBehavior::~CLockBehavior()
{
	CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyCreateNotify.c_str(), this);
	CMainSingleton::PostMaster().Unsubscribe(mySettings.myOnKeyInteractNotify.c_str(), this);
}

void CLockBehavior::Destroy()
{
}

void CLockBehavior::Awake()
{
}

void CLockBehavior::Start()
{
}

void CLockBehavior::Update()
{
}

void CLockBehavior::OnEnable()
{
}

void CLockBehavior::OnDisable()
{
}

void CLockBehavior::RunEvent()
{
	if (!myHasTriggered)
	{
		if (myMaxAmountOfKeys <= myAmountOfKeys)
		{
			myHasTriggered = true;
			CMainSingleton::PostMaster().Send({ mySettings.myOnNotify.c_str(), mySettings.myData });
		}
	}
}

void CLockBehavior::RunEventEditor()
{
	if (!myHasTriggered)
	{
		myHasTriggered = true;
		CMainSingleton::PostMaster().Send({ mySettings.myOnNotify.c_str(), mySettings.myData });
	}
}


void CLockBehavior::Receive(const SStringMessage& aMessage)
{
	if (aMessage.myMessageType == mySettings.myOnKeyCreateNotify.c_str())
		++myMaxAmountOfKeys;
	else
		++myAmountOfKeys;
}
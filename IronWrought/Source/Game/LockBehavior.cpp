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
	if (!myHasTriggered)
	{
		if (myAmountOfKeys >= myMaxAmountOfKeys)
		{
			RunEvent();
		}
	}
}

void CLockBehavior::OnEnable()
{
}

void CLockBehavior::OnDisable()
{
}

void CLockBehavior::RunEvent()
{
	std::cout << "---- Lock Activated \t" << GameObject().Name() << std::endl;
	myHasTriggered = true;
	CMainSingleton::PostMaster().Send({ mySettings.myOnNotify.c_str(), mySettings.myData });
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
	if (mySettings.myOnKeyCreateNotify.find(aMessage.myMessageType) != std::string::npos)
	{
		++myMaxAmountOfKeys;
		std::cout << "---- \t" << GameObject().Name() << " Keys: " << myMaxAmountOfKeys << std::endl;
	}
	else
	{
		++myAmountOfKeys;
	}
}



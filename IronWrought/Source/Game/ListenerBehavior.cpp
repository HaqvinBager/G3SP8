#include "stdafx.h"
#include "ListenerBehavior.h"
#include "ResponseBehavior.h"

CListenerBehavior::CListenerBehavior(CGameObject& aParent, const int aReceiveMessage) 
	: CBehavior(aParent)
	, myReceiveMessage(aReceiveMessage)
	, myHasTriggered(false)
{
}

CListenerBehavior::~CListenerBehavior()
{
	myResponses.clear();
}

void CListenerBehavior::OnEnable()
{
	CMainSingleton::PostMaster().Subscribe(myReceiveMessage, this);
}

void CListenerBehavior::OnDisable()
{
	CMainSingleton::PostMaster().Unsubscribe(myReceiveMessage, this);
}

void CListenerBehavior::Register(IResponseBehavior* aBehaviour)
{
	myResponses.push_back(aBehaviour);
}

void CListenerBehavior::TriggerResponses()
{
	std::cout << __FUNCTION__ << "Event Triggered: " << myReceiveMessage << "\n";
	for (auto& response : myResponses)
	{
		response->OnRespond();
	}
}

void CListenerBehavior::Receive(const SIDMessage& /*aMessage*/)
{
	if (Enabled())
	{
		if (!myHasTriggered)
		{
			myHasTriggered = true;
			TriggerResponses();
		}
	}
}

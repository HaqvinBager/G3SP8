#include "stdafx.h"
#include "ListenerComponent.h"
#include "ResponseBehavior.h"

CListenerComponent::CListenerComponent(CGameObject& aParent, std::string aReceiveMessage) : CBehavior(aParent), myReceiveMessage(aReceiveMessage), myHasTriggered(false)
{
}

CListenerComponent::~CListenerComponent()
{
}

void CListenerComponent::Awake()
{
}

void CListenerComponent::Start()
{
}

void CListenerComponent::Update()
{
}

void CListenerComponent::OnEnable()
{
	CMainSingleton::PostMaster().Subscribe(myReceiveMessage.c_str(), this);
}

void CListenerComponent::OnDisable()
{
	CMainSingleton::PostMaster().Unsubscribe(myReceiveMessage.c_str(), this);
}

void CListenerComponent::Register(IResponseBehavior* aBehaviour)
{
	myResponses.push_back(aBehaviour);
}

void CListenerComponent::Deregister(IResponseBehavior* aBehaviour)
{
	auto it = std::find(myResponses.begin(), myResponses.end(), aBehaviour);
	if (it != myResponses.end())
		myResponses.erase(it);
}

void CListenerComponent::TriggerResponses()
{
	std::cout << "Event Triggered: " << myReceiveMessage << "\n";
	for (auto& response : myResponses)
	{
		response->OnRespond();
	}
}

void CListenerComponent::Receive(const SStringMessage& /*aMessage*/)
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

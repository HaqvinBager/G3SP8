#include "stdafx.h"
#include "ListenerComponent.h"
#include "ResponseBehavior.h"

CListenerComponent::CListenerComponent(CGameObject& aParent, std::string aReceiveMessage) : CBehavior(aParent), myReceiveMessage(aReceiveMessage), myHasTriggered(false)
{
	CMainSingleton::PostMaster().Subscribe(myReceiveMessage.c_str(), this);
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
}

void CListenerComponent::OnDisable()
{
}

void CListenerComponent::TriggerResponses()
{
	for (auto& response : myResponses)
		response->OnRespond();
}

void CListenerComponent::Receive(const SStringMessage& /*aMessage*/)
{
	if (!myHasTriggered)
	{
		myHasTriggered = true;
		TriggerResponses();
	}
}

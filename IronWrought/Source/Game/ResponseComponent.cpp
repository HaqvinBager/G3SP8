#include "stdafx.h"
#include "ResponseComponent.h"

CResponseComponent::CResponseComponent(CGameObject& aParent, std::string aReceiveMessage) : CBehavior(aParent), myReceiveMessage(aReceiveMessage), myHasTriggered(false)
{
	CMainSingleton::PostMaster().Subscribe(myReceiveMessage.c_str(), this);
}

CResponseComponent::~CResponseComponent()
{
}

void CResponseComponent::Awake()
{
}

void CResponseComponent::Start()
{
}

void CResponseComponent::Update()
{
}

void CResponseComponent::OnEnable()
{
}

void CResponseComponent::OnDisable()
{
}

void CResponseComponent::Receive(const SStringMessage& /*aMessage*/)
{
	myHasTriggered = true;
	TriggerResponse();
}

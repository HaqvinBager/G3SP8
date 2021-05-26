#include "stdafx.h"
#include "ResponseComponent.h"

CResponseComponent::CResponseComponent(CGameObject& aParent, std::string aListenStringMessage) : CBehaviour(aParent), myListenStringMessage(aListenStringMessage), myHasTriggered(false)
{
	CMainSingleton::PostMaster().Subscribe(myListenStringMessage.c_str(), this);
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

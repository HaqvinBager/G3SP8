#include "stdafx.h"
#include "CustomEventComponent.h"
#include "CustomEventListenerComponent.h"


CCustomEventComponent::CCustomEventComponent(CGameObject& aParent, const char* anEventName)
	: CComponent(aParent)
	, myEventName(anEventName)
{

}

CCustomEventComponent::~CCustomEventComponent()
{
}

void CCustomEventComponent::Awake()
{
}

void CCustomEventComponent::Start()
{
}

void CCustomEventComponent::Update()
{
	if (Input::GetInstance()->IsKeyPressed('K'))
	{
		OnRaiseEvent();
	}
}

void CCustomEventComponent::Register(CCustomEventListenerComponent* aListener)
{
	if (std::find(myListeners.begin(), myListeners.end(), aListener) != myListeners.end())
		return;

	myListeners.push_back(aListener);
}

void CCustomEventComponent::UnRegister(CCustomEventListenerComponent* aListener)
{
	std::vector<CCustomEventListenerComponent*>::iterator it = std::find(myListeners.begin(), myListeners.end(), aListener);
	if (it != myListeners.end())
		myListeners.erase(it);
}

void CCustomEventComponent::OnRaiseEvent()
{
	for (auto& listener : myListeners)
	{
		listener->OnCustomEventRaised();
	}
}

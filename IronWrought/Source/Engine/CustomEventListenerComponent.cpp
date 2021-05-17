#include "stdafx.h"
#include "CustomEventListenerComponent.h"
#include "CustomEventComponent.h"
#include "PointLightComponent.h"


CCustomEventListenerComponent::CCustomEventListenerComponent(CGameObject& aParent, CCustomEventComponent* aCustomEvent)
	: CBehaviour(aParent)
	, myCustomEvent(aCustomEvent)
{


}

CCustomEventListenerComponent::~CCustomEventListenerComponent()
{
}

void CCustomEventListenerComponent::Awake()
{
	CPointLightComponent* pl = nullptr;
	if (GameObject().TryGetComponent(&pl))
	{
		myResponse = [pl]() { pl->Enabled(!pl->Enabled()); };
	}
}

void CCustomEventListenerComponent::Start()
{
}

void CCustomEventListenerComponent::Update()
{
}

void CCustomEventListenerComponent::OnEnable()
{
	std::cout << myCustomEvent->Name() << " += " << GameObject().Name() << std::endl;
	myCustomEvent->Register(this);
}

void CCustomEventListenerComponent::OnDisable()
{
	std::cout << myCustomEvent->Name() << " -= " << GameObject().Name() << std::endl;
	myCustomEvent->UnRegister(this);
}

void CCustomEventListenerComponent::OnCustomEventRaised()
{
	std::cout << "Listener Raised Event: " << myCustomEvent->Name() << std::endl;
	if (myResponse != nullptr)
	{
		myResponse();
	}
}

#include "stdafx.h"
#include "IListenerComponent.h"
#include "GameEvent.h"

IListenerComponent::IListenerComponent(CGameObject& aParent)
	: CComponent(aParent)
{

}

IListenerComponent::~IListenerComponent()
{
}

void IListenerComponent::ConnectToGameEvent(std::weak_ptr<CGameEvent> aGameEvent)
{
	myGameEvent = aGameEvent;
}

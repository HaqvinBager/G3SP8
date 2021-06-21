#include "stdafx.h"
#include "GameEvent.h"

CGameEvent::CGameEvent(CGameObject& aParent) 
	: CBehavior(aParent)
{
}

CGameEvent::~CGameEvent()
{
}

void CGameEvent::Register(IListener* aListener)
{
	auto it = std::find(myListeners.begin(), myListeners.end(), aListener);
	if (it == myListeners.end())
		myListeners.push_back(aListener);
}

void CGameEvent::UnRegister(IListener* aListener)
{
	auto it = std::find(myListeners.begin(), myListeners.end(), aListener);
	if (it != myListeners.end())
		myListeners.erase(it);
}

void CGameEvent::OnRaiseEvent()
{
	for (auto& listener : myListeners)
	{
		listener->OnEventRaised();
	}
}

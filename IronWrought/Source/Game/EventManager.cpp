#include "stdafx.h"
#include "EventManager.h"
#include "GameEvent.h"
#include "Scene.h"
#include "IListenerComponent.h"


CEventManager::CEventManager(CGameObject& aParent)
	: CComponent(aParent)
{

}

CEventManager ::~CEventManager()
{

}


//std::weak_ptr<CGameEvent> EventManager::GetCreateGameEvent(const int aInstanceID)
//{
//	//auto it = std::find(myEvents.begin(), myEvents.end(), aInstanceID);
//	for (auto it = myEvents.begin(); it != myEvents.end(); ++it)
//	{
//		if ((*it)->InstanceID() == aInstanceID)
//		{
//			(*it)->Register()
//		}
//	}
//
//
//	return std::weak_ptr<CGameEvent>();
//}

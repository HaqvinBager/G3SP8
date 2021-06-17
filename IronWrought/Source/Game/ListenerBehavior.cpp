#include "stdafx.h"
#include "ListenerBehavior.h"
#include "ResponseBehavior.h"
#include "LockBehavior.h"
#include "Scene.h"

CListenerBehavior::CListenerBehavior(CGameObject& aParent, const int aReceiveMessage)
	: CBehavior(aParent)
	, myReceiveMessage(aReceiveMessage)
	, myHasTriggered(false)
	, myLockInstanceID(-1)
{
}

CListenerBehavior::CListenerBehavior(CGameObject& aParent, const int aReceiveMessage, const int aLockInstanceID) 
	: CBehavior(aParent)
	, myReceiveMessage(aReceiveMessage)
	, myHasTriggered(false)
	, myLockInstanceID(aLockInstanceID)
{
}

CListenerBehavior::~CListenerBehavior()
{
	myResponses.clear();
}

void CListenerBehavior::OnEnable()
{
	CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myLockInstanceID);
	if (lockGameObject != nullptr)
	{
		CLockBehavior* lock = nullptr;
		if (lockGameObject->TryGetComponentAny(&lock))
		{
			lock->Register(this);
		}
	}
	//CMainSingleton::PostMaster().Subscribe(myReceiveMessage, this);
}

void CListenerBehavior::OnDisable()
{
	CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myLockInstanceID);
	if (lockGameObject != nullptr)
	{
		CLockBehavior* lock = nullptr;
		if (lockGameObject->TryGetComponentAny(&lock))
		{
			lock->Unregister(this);
		}
	}
	//CMainSingleton::PostMaster().Unsubscribe(myReceiveMessage, this);
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

void CListenerBehavior::OnRaiseEvent()
{
	std::cout << __FUNCTION__ << " -> from gameObject: " << GameObject().Name() << std::endl;
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

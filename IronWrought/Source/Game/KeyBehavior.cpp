#include "stdafx.h"
#include "KeyBehavior.h"
#include "ActivationBehavior.h"
#include "Scene.h"
#include "LockBehavior.h"
#include "LeftClickDownLock.h"
#include "OnTriggerLock.h"
#include "OnLookLock.h"

CKeyBehavior::CKeyBehavior(CGameObject& aParent, const SSettings& someSettings)
	: CBehavior(aParent)
	, mySettings(someSettings)
{
	//CMainSingleton::PostMaster().Send({ mySettings.myOnCreateNotifyName.c_str(), mySettings.myOnCreateNotify });
}

CKeyBehavior::~CKeyBehavior()
{
}

void CKeyBehavior::Awake()
{
}

void CKeyBehavior::Start()
{
}

void CKeyBehavior::Update()
{
}

void CKeyBehavior::Register(IActivationBehavior* aBehaviour)
{
	std::cout << __FUNCTION__ << "----< \tActivation type registered \t" << aBehaviour->GameObject().Name() << std::endl;
	if (std::find(myActivations.begin(), myActivations.end(), aBehaviour) == myActivations.end())
	{
		aBehaviour->SetHasLock(mySettings.myHasLock);
		myActivations.push_back(aBehaviour);
	}
}

void CKeyBehavior::TriggerActivations()
{
	if (Enabled())
	{
		std::cout << __FUNCTION__ << "----< \t Key triggered \t" << GameObject().Name() << std::endl;
		for (auto& activation : myActivations)
		{
			activation->OnActivation();
		}

		CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myLockInstanceID);
		if (lockGameObject != nullptr)
		{
			CLockBehavior* lock = nullptr;
			if (lockGameObject->TryGetComponentAny(&lock))
			{
				lock->OnKeyActivated(this);
				Enabled(false);
			}
		}

		//CMainSingleton::PostMaster().Send({ mySettings.myInteractNotifyName, mySettings.myInteractNotify });
		//
		//if(mySettings.myHasLock == 1)
		//	Enabled(false);
	}
}

void CKeyBehavior::OnEnable()
{
	CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myLockInstanceID);
	if (lockGameObject != nullptr)
	{
		CLockBehavior* lock = nullptr;
		if (lockGameObject->TryGetComponentAny(&lock))
		{
			lock->AddKey(this);
		}
	}
}

void CKeyBehavior::OnDisable()
{
	CGameObject* lockGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myLockInstanceID);
	if (lockGameObject != nullptr)
	{
		CLockBehavior* lock = nullptr;
		if (lockGameObject->TryGetComponentAny(&lock))
		{
			lock->RemoveKey(this);
		}
	}
}

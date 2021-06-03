#include "stdafx.h"
#include "KeyBehavior.h"
#include "ActivationBehavior.h"

CKeyBehavior::CKeyBehavior(CGameObject& aParent, const SSettings& someSettings) : CBehavior(aParent), mySettings(someSettings)
{
	CMainSingleton::PostMaster().Send({ mySettings.myOnCreateNotify.c_str(), mySettings.myData });
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
	aBehaviour->SetHasLock(mySettings.myHasLock);
	myActivations.push_back(aBehaviour);
}

void CKeyBehavior::TriggerActivations()
{
	if (Enabled())
	{
		for (auto& activation : myActivations)
		{
			std::cout << __FUNCTION__ << "----< \tActivation type triggered \t" << activation->GameObject().Name() << std::endl;
			activation->OnActivation();
		}
		CMainSingleton::PostMaster().Send({ mySettings.myInteractNotify.c_str(), mySettings.myData });
		
		if(mySettings.myHasLock == 1)
			Enabled(false);
	}
}

void CKeyBehavior::OnEnable()
{
}

void CKeyBehavior::OnDisable()
{
}

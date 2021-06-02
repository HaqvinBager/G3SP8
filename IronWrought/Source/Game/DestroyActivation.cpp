#include "stdafx.h"
#include "DestroyActivation.h"
#include "RigidBodyComponent.h"
#include "Scene.h"

CDestroyActivation::CDestroyActivation(CGameObject& aParent) : IActivationBehavior(aParent)
{

}

CDestroyActivation::~CDestroyActivation()
{

}

void CDestroyActivation::OnActivation()
{/*
	std::cout << __FUNCTION__ << "----> \tKey: " << GameObject().Name() << " Code: " << mySettings.myInteractNotify << std::endl;

	CMainSingleton::PostMaster().Send({ mySettings.myInteractNotify.c_str(), mySettings.myData });*/
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

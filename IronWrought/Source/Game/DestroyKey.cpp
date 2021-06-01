#include "stdafx.h"
#include "DestroyKey.h"
#include "RigidBodyComponent.h"
#include "Scene.h"

CDestroyKey::CDestroyKey(CGameObject& aParent, const SSettings& someSettings) : CKeyBehavior(aParent, someSettings)
{

}

CDestroyKey::~CDestroyKey()
{

}

void CDestroyKey::OnInteract()
{
	std::cout << "----> \tKey: " << GameObject().Name() << " Code: " << mySettings.myInteractNotify << std::endl;

	CMainSingleton::PostMaster().Send({ mySettings.myInteractNotify.c_str(), mySettings.myData });
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

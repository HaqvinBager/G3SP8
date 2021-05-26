#include "stdafx.h"
#include "KeyComponent.h"
#include "RigidBodyComponent.h"
#include "Scene.h"

CKeyComponent::CKeyComponent(CGameObject& aParent, std::string aCreateMessage, std::string aDestroyMessage, void* someData) : CBehaviour(aParent), myCreateStringMessage(aCreateMessage), myPickUpStringMessage(aDestroyMessage), myData(someData)
{
	CMainSingleton::PostMaster().Send({ myCreateStringMessage.c_str(), myData });

}

CKeyComponent::~CKeyComponent()
{
}

void CKeyComponent::Destroy()
{
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

void CKeyComponent::Awake()
{
}

void CKeyComponent::Start()
{
}

void CKeyComponent::Update()
{
}

void CKeyComponent::OnEnable()
{
}

void CKeyComponent::OnDisable()
{
}

void CKeyComponent::OnPickUp()
{
	CMainSingleton::PostMaster().Send({ myPickUpStringMessage.c_str(), myData });
	Destroy();
}

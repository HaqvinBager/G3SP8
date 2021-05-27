#include "stdafx.h"
#include "KeyComponent.h"
#include "RigidBodyComponent.h"
#include "Scene.h"

CKeyComponent::CKeyComponent(CGameObject& aParent, std::string aCreateSendMessage, std::string aPickUpSendMessage, void* someData) : CBehavior(aParent), myCreateSendMessage(aCreateSendMessage), myPickUpSendMessage(aPickUpSendMessage), myData(someData)
{
	CMainSingleton::PostMaster().Send({ myCreateSendMessage.c_str(), myData });

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
	CMainSingleton::PostMaster().Send({ myPickUpSendMessage.c_str(), myData });
	Destroy();
}

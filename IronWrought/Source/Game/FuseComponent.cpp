#include "stdafx.h"
#include "FuseComponent.h"
#include "RigidBodyComponent.h"
#include "Engine.h"
#include "Scene.h"

CFuseComponent::CFuseComponent(CGameObject& aParent) : CBehaviour(aParent)
{
	int value = 1;
	CMainSingleton::PostMaster().Send({ EMessageType::FuseCreated, &value });
}

CFuseComponent::~CFuseComponent()
{
}

void CFuseComponent::Destroy()
{
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

void CFuseComponent::Awake()
{
}

void CFuseComponent::Start()
{
}

void CFuseComponent::Update()
{

}

void CFuseComponent::OnEnable()
{
}

void CFuseComponent::OnDisable()
{
}

void CFuseComponent::OnPickUp()
{
	int value = 1;
	CMainSingleton::PostMaster().Send({ EMessageType::FusePickedUp, &value });
	this->Destroy();
}

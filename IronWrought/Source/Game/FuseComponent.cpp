#include "stdafx.h"
#include "FuseComponent.h"
#include "RigidBodyComponent.h"
#include "Scene.h"
#include "PickupComponent.h"

CFuseComponent::CFuseComponent(CGameObject& aParent) : CPickupComponent(aParent)
{
	int value = 1;
	CMainSingleton::PostMaster().Send({ EMessageType::FuseCreated, &value });
}

CFuseComponent::~CFuseComponent()
{
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

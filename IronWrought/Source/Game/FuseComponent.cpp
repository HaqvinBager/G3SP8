#include "stdafx.h"
#include "FuseComponent.h"
#include "RigidBodyComponent.h"
#include "Scene.h"
//#include "KeyComponent.h"
#include <PointLightComponent.h>

CFuseComponent::CFuseComponent(CGameObject& /*aParent*/)/* : CKeyComponent(aParent)*/
{
	int value = 1;
	CMainSingleton::PostMaster().Send({ EMessageType::FuseCreated, &value });
}

CFuseComponent::~CFuseComponent()
{
}/*

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

	CPointLightComponent* pl = nullptr;
	if (GameObject().TryGetComponent(&pl))
	{
		pl->Enabled(false);
	}

	this->Destroy();
}*/

#include "stdafx.h"
#include "FuseboxComponent.h"
#include "Engine.h"
#include "Scene.h"

CFuseboxComponent::CFuseboxComponent(CGameObject& aParent) : CBehaviour(aParent), myNumberOfFuses(0), myNumberOfPickedUpFuses(0), myHasTriggered(false)
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::FuseCreated, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::FusePickedUp, this);
}

CFuseboxComponent::~CFuseboxComponent()
{
}

void CFuseboxComponent::Destroy()
{
}

void CFuseboxComponent::Awake()
{
}

void CFuseboxComponent::Start()
{
}

void CFuseboxComponent::Update()
{
}

void CFuseboxComponent::OnEnable()
{
}

void CFuseboxComponent::OnDisable()
{
}

void CFuseboxComponent::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::FuseCreated:
	{
		++myNumberOfFuses;
	}
	break;
	case EMessageType::FusePickedUp:
	{
		++myNumberOfPickedUpFuses;
	}
	break;
	default:
		break;
	}
}

void CFuseboxComponent::RunEvent()
{
	if (!myHasTriggered)
	{
		if (myNumberOfFuses == myNumberOfPickedUpFuses)
		{
			std::cout << "Hello!" << std::endl;
			myHasTriggered = true;
			CMainSingleton::PostMaster().Send({ EMessageType::OpenSafetyDoors, &myHasTriggered });
		}
	}
}

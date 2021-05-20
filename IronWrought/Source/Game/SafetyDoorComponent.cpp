#include "stdafx.h"
#include "SafetyDoorComponent.h"
#include "TransformComponent.h"

CSafetyDoorComponent::CSafetyDoorComponent(CGameObject& aParent) : CBehaviour(aParent), myShouldOpenDoors(false), myIsOpen(false)
{
	myOpenDoorPosition = GameObject().myTransform->Position().y - 10.0f;
	CMainSingleton::PostMaster().Subscribe(EMessageType::OpenSafetyDoors, this);
}

CSafetyDoorComponent::~CSafetyDoorComponent()
{
}

void CSafetyDoorComponent::Destroy()
{
}

void CSafetyDoorComponent::Awake()
{
}

void CSafetyDoorComponent::Start()
{
}

void CSafetyDoorComponent::Update()
{
	if (!myIsOpen)
	{
		if (myShouldOpenDoors)
		{
			Vector3 newPosition = GameObject().myTransform->Position();
			if (newPosition.y >= myOpenDoorPosition)
			{
				newPosition.y -= 5.0f * CTimer::Dt();
				GameObject().myTransform->Move(newPosition);
			}
			else
			{
				myIsOpen = true;
			}
		}
	}
}

void CSafetyDoorComponent::OnEnable()
{
}

void CSafetyDoorComponent::OnDisable()
{
}

void CSafetyDoorComponent::Receive(const SMessage& aMessage)
{
	switch (aMessage.myMessageType)
	{
	case EMessageType::OpenSafetyDoors:
	{
		myShouldOpenDoors = true;
	}
	break;
	default:
		break;
	}
}
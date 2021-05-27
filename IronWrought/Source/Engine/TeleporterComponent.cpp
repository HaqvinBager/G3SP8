#include "stdafx.h"
#include "TeleporterComponent.h"
#include "PhysXWrapper.h"
#include "TransformComponent.h"
#include <PlayerControllerComponent.h>
#include "MainSingleton.h"

CTeleporterComponent::CTeleporterComponent(CGameObject& aParent
										   , const ELevelName aNameOfTeleporter
										   , const ELevelName aNameOfTeleportTo
										   , const Vector3& aPosOnTeleportTo
) : CBehavior(aParent)
, 	myName(aNameOfTeleporter)
,	myTeleportTo(aNameOfTeleportTo)
,	myOnTeleportToMe(aPosOnTeleportTo)
,	myTeleportTimer(0.0f)
,	myTimeUntilTeleport(0.5f)
,	myTeleportInProgress(false)
,	myTeleportTarget(nullptr)
{
	CMainSingleton::PostMaster().Subscribe(EMessageType::Teleport, this);
}

CTeleporterComponent::~CTeleporterComponent()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Teleport, this);
}

void CTeleporterComponent::Awake()
{}

void CTeleporterComponent::Start()
{}

void CTeleporterComponent::Update()
{
	if (myTeleportInProgress)
	{
		myTeleportTimer -= CTimer::Dt();
		if (myTeleportTimer <= 0.0f)
		{
			HandleTeleport();
		}
	}
}

void CTeleporterComponent::OnEnable()
{}

void CTeleporterComponent::OnDisable()
{}

void CTeleporterComponent::Receive(const SMessage& aMessage)
{
	if (aMessage.myMessageType == EMessageType::Teleport)
	{
		STeleportData& teleportData = *static_cast<STeleportData*>(aMessage.data);
		if (teleportData.myTarget == myName && myTeleportInProgress == false)
		{
			if (!teleportData.myTransformToTeleport)
				return;

			myTeleportInProgress = teleportData.myStartTeleport;
			myTeleportTarget = teleportData.myTransformToTeleport;
			myTeleportTimer = myTimeUntilTeleport;
			// Trigger camera fade to black... ?

			teleportData.Reset();
			return;
		}

		if (teleportData.myStartTeleport == false)
		{
			myTeleportInProgress = false;
			myTeleportTarget = nullptr;
			return;
		}
	}
}

void CTeleporterComponent::TriggerTeleport(const bool aEnterCollider, CTransformComponent* aTarget)
{
	STeleportData teleportData = { aTarget, myTeleportTo, aEnterCollider };
	CMainSingleton::PostMaster().Send({ EMessageType::Teleport, &teleportData });
}

void CTeleporterComponent::HandleTeleport()
{
	// Debug Code: OK TO REMOVE
	//std::cout << "teleport To: " << myOnTeleportToMe.x << " " << myOnTeleportToMe.y << " " << myOnTeleportToMe.z << std::endl;
	//std::cout << "target: " << myTeleportTarget->Position().x << " " << myTeleportTarget->Position().y << " " << myTeleportTarget->Position().z << std::endl;
	//if (myTeleportTarget->GameObject().GetComponent<CPlayerControllerComponent>())
	//{
	//	std::cout << "Is Player " << std::endl;
	//}
	//std::cout << static_cast<int>(myName) << std::endl;
	//std::cout << "----" << std::endl;

 
	myTeleportTarget->CopyRotation(GameObject().myTransform->Transform());
	// Currently only affects player.
	myTeleportTarget->GameObject().GetComponent<CPlayerControllerComponent>()->SetControllerPosition(myOnTeleportToMe);
	myTeleportInProgress = false;
}

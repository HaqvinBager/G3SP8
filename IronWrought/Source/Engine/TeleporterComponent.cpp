#include "stdafx.h"
#include "TeleporterComponent.h"
#include "PhysXWrapper.h"
#include "TransformComponent.h"
#include <PlayerControllerComponent.h>
#include "MainSingleton.h"

CTeleporterComponent::CTeleporterComponent(CGameObject& aParent
										   , const PostMaster::ELevelName aNameOfTeleporter
										   , const PostMaster::ELevelName aNameOfTeleportTo
										   , const Vector3& aPosOnTeleportTo
) : CBehavior(aParent)
, 	myName(aNameOfTeleporter)
,	myTeleportTo(aNameOfTeleportTo)
,	myOnTeleportToMe(aPosOnTeleportTo)
,	myTeleportTimer(0.0f)
,	myTimeUntilTeleport(0.1f)
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
	using namespace PostMaster;
	if (aMessage.myMessageType == EMessageType::Teleport)
	{
		STeleportData& teleportData = *static_cast<STeleportData*>(aMessage.data);
		if (teleportData.myTarget == myName && myTeleportInProgress == false)
		{
			if (!teleportData.myTransformToTeleport)
				return;

			GameObject().Active(true);

			myTeleportInProgress = teleportData.myStartTeleport;
			myTeleportTarget = teleportData.myTransformToTeleport;
			myTeleportTimer = myTimeUntilTeleport;
			// Trigger camera fade to black... ?

			teleportData.Reset();
			return;
		}

		if (teleportData.myStartTeleport == false)
		{
			GameObject().Active(false);

			myTeleportInProgress = false;
			myTeleportTarget = nullptr;
			return;
		}
	}
}

void CTeleporterComponent::TriggerTeleport(const bool aEnterCollider, CTransformComponent* aTarget)
{
	PostMaster::STeleportData teleportData = { aTarget, myTeleportTo, aEnterCollider };
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

	if (!myTeleportTarget)
		return;

	myTeleportTarget->CopyRotation(GameObject().myTransform->Transform());
	// Currently only affects player.
	myTeleportTarget->GameObject().GetComponent<CPlayerControllerComponent>()->SetControllerPosition(myOnTeleportToMe);
	myTeleportTarget->GameObject().GetComponent<CPlayerControllerComponent>()->SetRespawnPosition();

	myTeleportTarget = nullptr;
	myTeleportInProgress = false;
}

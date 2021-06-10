#include "stdafx.h"
#include "TeleportActivation.h"

#include "PhysXWrapper.h"
#include "TransformComponent.h"
#include "PlayerControllerComponent.h"
#include "MainSingleton.h"
#include "Scene.h"
#include "CameraComponent.h"

#define PI 3.141592f

CTeleportActivation::CTeleportActivation(
	CGameObject& aParent
	, const PostMaster::ELevelName aNameOfTeleporter
	, const PostMaster::ELevelName aNameOfTeleportTo
	, const Vector3& aPosOnTeleportTo
	, const Vector3& aRotOnTeleportTo
	, const float aTimeUntilTeleport
)
	: IActivationBehavior(aParent)
	, 	myName(aNameOfTeleporter)
	,	myTeleportTo(aNameOfTeleportTo)
	,	myOnTeleportToMePosition(aPosOnTeleportTo)
	,	myTeleportTimer(aTimeUntilTeleport)
	,	myHasTeleported(false)

{
	myOnTeleportToMeRotation = aRotOnTeleportTo;
	myOnTeleportToMeRotation.x = (-myOnTeleportToMeRotation.x) - 360.0f;
	myOnTeleportToMeRotation.y += 180.0f;
	myOnTeleportToMeRotation.z = (-myOnTeleportToMeRotation.z) - 360.0f;
	myOnTeleportToMeRotation *= (PI / 180.0f);

	CMainSingleton::PostMaster().Subscribe(EMessageType::Teleport, this);
}

CTeleportActivation::~CTeleportActivation()
{
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::Teleport, this);
}

void CTeleportActivation::Update()
{
	if (myIsInteracted)
	{	
		myTeleportTimer -= CTimer::Dt();
		// Do once:
			// Fade Camera
			// Lock Player Movement (could be made to support transform component. But is not needed atm so support won't be added.)
	}

	if (Complete(myTeleportTimer <= 0.0f))
	{
		if (myHasTeleported)
			return;

		CTransformComponent* aTargetToTeleport = IRONWROUGHT->GetActiveScene().Player()->myTransform;
		PostMaster::STeleportData teleportData = { aTargetToTeleport, myTeleportTo, true };
		CMainSingleton::PostMaster().Send({ EMessageType::Teleport, &teleportData });
		myHasTeleported = true;
	}
}

void CTeleportActivation::Receive(const SMessage & aMessage)
{
	using namespace PostMaster;
	if (aMessage.myMessageType == EMessageType::Teleport)
	{
		STeleportData& teleportData = *static_cast<STeleportData*>(aMessage.data);
		if (teleportData.myTarget == myName)
		{
			HandleTeleport(teleportData.myTransformToTeleport);
			teleportData.Reset();
			PostMaster::SBoxColliderEvenTriggerData data;
			data.myState = true;
			data.mySceneSection = PostMaster::LevelNameToSection(myName);
			CMainSingleton::PostMaster().Send({ PostMaster::SMSG_SECTION, &data });
			return;
		}
	}
}

void CTeleportActivation::HandleTeleport(CTransformComponent* aTargetToTeleport)
{
	// Debug Code: OK TO REMOVE
	//std::cout << "teleport To: " << myOnTeleportToMePosition.x << " " << myOnTeleportToMePosition.y << " " << myOnTeleportToMePosition.z << std::endl;
	//td::cout << "target: " << aTargetToTeleport->Position().x << " " << aTargetToTeleport->Position().y << " " << aTargetToTeleport->Position().z << std::endl;
	//f (aTargetToTeleport->GameObject().GetComponent<CPlayerControllerComponent>())
	//
	//	std::cout << "Is Player " << std::endl;
	//
	//td::cout << static_cast<int>(myName) << std::endl;
	//td::cout << "----" << std::endl;

	if (!aTargetToTeleport)
		return;

	IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, myTeleportTimer);
	std::cout << __FUNCTION__ << " " << myTeleportTimer << std::endl;
	aTargetToTeleport->Rotation(myOnTeleportToMeRotation);
	aTargetToTeleport->Position(myOnTeleportToMePosition);
	CPlayerControllerComponent* player = nullptr;
	if (aTargetToTeleport->GameObject().TryGetComponent<CPlayerControllerComponent>(&player))
	{
		player->SetControllerPosition(myOnTeleportToMePosition);
		player->SetRespawnPosition();
	}
}

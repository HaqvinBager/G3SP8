#include "stdafx.h"
#include "TeleportResponse.h"

#include "PhysXWrapper.h"
#include "TransformComponent.h"
#include "PlayerControllerComponent.h"
#include "MainSingleton.h"
#include "Scene.h"
#include "CameraComponent.h"

#define PI 3.141592f

CTeleportResponse::CTeleportResponse(
	CGameObject& aParent
	, const float aDelay, const std::string& aTarget
)
	: IResponseBehavior(aParent)
	, myDelay(aDelay)
	, myTarget(aTarget)

{
}

CTeleportResponse::~CTeleportResponse()
{
}

void CTeleportResponse::Awake()
{
}

void CTeleportResponse::Start()
{
	//CMainSingleton::PostMaster().Subscribe(EMessageType::Teleport, this);
	HasBeenActivated(false);
}

void CTeleportResponse::Update()
{
	//if (!HasBeenActivated())
	//	return;

	//myTeleportTimer -= CTimer::Dt();
	//if (!myActivated)
	//{
	//	IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, myTeleportTimer);
	//	IRONWROUGHT->GetActiveScene().PlayerController()->LockMovementFor(myTeleportTimer);
	//	myActivated = true;
	//}

	//if (myTeleportTimer <= 0.0f)
	//{
	//	if (myHasTeleported)
	//		return;

	//	CTransformComponent* aTargetToTeleport = IRONWROUGHT->GetActiveScene().Player()->myTransform;
	//	PostMaster::STeleportData teleportData = { aTargetToTeleport, myTeleportTo, true };
	//	CMainSingleton::PostMaster().Send({ EMessageType::Teleport, &teleportData });
	//	myHasTeleported = true;
	//}
}

//void CTeleportResponse::Receive(const SMessage& aMessage)
//{
	//switch (aMessage.myMessageType)
	//{
	//case EMessageType::Teleport:
	//	OnRespond();
	//	break;
	//default:
	//	break;
	//}
	//using namespace PostMaster;
	//if (aMessage.myMessageType == EMessageType::Teleport)
	//{
	//	STeleportData& teleportData = *static_cast<STeleportData*>(aMessage.data);
	//	if (teleportData.myTarget == myName)
	//	{
	//		HandleTeleport(teleportData.myTransformToTeleport);
	//		teleportData.Reset();
	//		PostMaster::SBoxColliderEvenTriggerData data;
	//		data.myState = true;
	//		data.mySceneSection = PostMaster::LevelNameToSection(myName);
	//		CMainSingleton::PostMaster().Send({ PostMaster::SMSG_SECTION, &data });
	//		return;
	//	}
	//}
//}

void CTeleportResponse::OnRespond()
{
	CMainSingleton::PostMaster().Send({ EMessageType::LoadLevel, &myTarget });
	//CMainSingleton::PostMaster().Unsubscribe(EMessageType::Teleport, this);
}
//
//void CTeleportResponse::HandleTeleport(CTransformComponent* aTargetToTeleport)
//{
//	// Debug Code: OK TO REMOVE
//	//std::cout << "teleport To: " << myOnTeleportToMePosition.x << " " << myOnTeleportToMePosition.y << " " << myOnTeleportToMePosition.z << std::endl;
//	//td::cout << "target: " << aTargetToTeleport->Position().x << " " << aTargetToTeleport->Position().y << " " << aTargetToTeleport->Position().z << std::endl;
//	//f (aTargetToTeleport->GameObject().GetComponent<CPlayerControllerComponent>())
//	//
//	//	std::cout << "Is Player " << std::endl;
//	//
//	//td::cout << static_cast<int>(myName) << std::endl;
//	//td::cout << "----" << std::endl;
//
//	if (!aTargetToTeleport)
//		return;
//
//	aTargetToTeleport->Rotation(myOnTeleportToMeRotation);
//	aTargetToTeleport->Position(myOnTeleportToMePosition);
//	CPlayerControllerComponent* player = nullptr;
//	if (aTargetToTeleport->GameObject().TryGetComponent<CPlayerControllerComponent>(&player))
//	{
//		player->SetControllerPosition(myOnTeleportToMePosition);
//		player->SetRespawnPosition();
//	}
//}

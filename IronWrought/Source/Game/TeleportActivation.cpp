#include "stdafx.h"
#include "TeleportActivation.h"

//#include "PhysXWrapper.h"
//#include "TransformComponent.h"
//#include "PlayerControllerComponent.h"
//#include "MainSingleton.h"
//#include "Scene.h"
//#include "CameraComponent.h"

CTeleportActivation::CTeleportActivation(CGameObject& aParent, const float aDelay, const std::string& aTarget)
	: IActivationBehavior(aParent)
	, myDelay(aDelay)
	, myTarget(aTarget)
{
}

CTeleportActivation::~CTeleportActivation()
{
}

void CTeleportActivation::Update()
{

}

void CTeleportActivation::OnActivation()
{
	CMainSingleton::PostMaster().Send({ myTarget.c_str(), nullptr});
}

//void CTeleportActivation::Receive(const SMessage & aMessage)
//{
//	using namespace PostMaster;
//	if (aMessage.myMessageType == EMessageType::Teleport)
//	{
//		STeleportData& teleportData = *static_cast<STeleportData*>(aMessage.data);
//		if (teleportData.myTarget == myName)
//		{
//			HandleTeleport(teleportData.myTransformToTeleport);
//			teleportData.Reset();
//			PostMaster::SBoxColliderEvenTriggerData data;
//			data.myState = true;
//			data.mySceneSection = PostMaster::LevelNameToSection(myName);
//			CMainSingleton::PostMaster().Send({ PostMaster::SMSG_SECTION, &data });
//			return;
//		}
//	}
//}
//
//void CTeleportActivation::HandleTeleport(CTransformComponent* aTargetToTeleport)
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
//	std::cout << __FUNCTION__ << " " << myTeleportTimer << std::endl;
//	aTargetToTeleport->Rotation(myOnTeleportToMeRotation);
//	aTargetToTeleport->Position(myOnTeleportToMePosition);
//	CPlayerControllerComponent* player = nullptr;
//	if (aTargetToTeleport->GameObject().TryGetComponent<CPlayerControllerComponent>(&player))
//	{
//		player->SetControllerPosition(myOnTeleportToMePosition);
//		player->SetRespawnPosition();
//	}
//}


/*if (myIsInteracted)
{
	myTeleportTimer -= CTimer::Dt();
	if (!myActivated)
	{
		IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, myTeleportTimer);
		IRONWROUGHT->GetActiveScene().PlayerController()->LockMovementFor(myTeleportTimer);
		myActivated = true;
	}
}

if (Complete(myTeleportTimer <= 0.0f))
{
	if (myHasTeleported)
		return;

	CTransformComponent* aTargetToTeleport = IRONWROUGHT->GetActiveScene().Player()->myTransform;
	PostMaster::STeleportData teleportData = { aTargetToTeleport, myTeleportTo, true };
	CMainSingleton::PostMaster().Send({ EMessageType::Teleport, &teleportData });
	myHasTeleported = true;
}*/
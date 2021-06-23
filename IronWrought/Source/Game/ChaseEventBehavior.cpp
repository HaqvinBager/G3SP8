#include "stdafx.h"
#include "ChaseEventBehavior.h"
#include "AIController.h"
#include "TransformComponent.h"
#include "CameraControllerComponent.h"
#include "Scene.h"
#include "PlayerControllerComponent.h"
#include "CameraComponent.h"

CChaseEventBehavior::CChaseEventBehavior(CGameObject& aParent, CChaseEventBehavior::SSettings someSettings, SNavMesh* aNavMesh)
	: CBehavior(aParent)
	, mySettings(someSettings)
	, myNavMesh(aNavMesh)
	, myAttackPlayerTimer(0.0f)
	, myDetachedPlayerHead(nullptr)
	, myPlayer(nullptr)
	, myCurrentBehavior(0)
	, myMovementLocked(false)
{
}

CChaseEventBehavior::~CChaseEventBehavior()
{
	myDetachedPlayerHead = nullptr;
	myPlayer = nullptr;
	myNavMesh = nullptr;
	myBehaviors.clear();
}


void CChaseEventBehavior::Awake()
{
}

void CChaseEventBehavior::Start()
{
	myPlayer = CEngine::GetInstance()->GetActiveScene().Player();
	myBehaviors.push_back(new CSeek(myNavMesh, mySettings.mySpawnPosition.y));
	dynamic_cast<CSeek*>(myBehaviors.back())->SetTarget(myPlayer->myTransform);
}

void CChaseEventBehavior::Update()
{
	myBehaviors[myCurrentBehavior]->Update(GameObject().myTransform->Position());
}

void CChaseEventBehavior::OnEnable()
{
}

void CChaseEventBehavior::OnDisable()
{
}

void CChaseEventBehavior::Vignette()
{
}

void CChaseEventBehavior::Shake()
{
}

void CChaseEventBehavior::ChangeState()
{
}

void CChaseEventBehavior::UpdateAttackEvent()
{
	myAttackPlayerTimer -= CTimer::Dt();
	if (myAttackPlayerTimer <= 0.0f)
	{
		IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, 0.1f, false);
		float damageToPlayer = 34.0f;
		CMainSingleton::PostMaster().Send({ EMessageType::PlayerTakeDamage, &damageToPlayer });
		CPlayerControllerComponent* plCtrl = myPlayer->GetComponent<CPlayerControllerComponent>();
		myDetachedPlayerHead->SetParent(myPlayer->myTransform);
		plCtrl->ForceCrouch();
		//plCtrl->OnCrouch();

		bool lockCamera = false;
		CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
		//CMainSingleton::PostMaster().Send({ PostMaster::SMSG_ENABLE_GLOVE, nullptr });// Only needs to be re-enabled if it has been disabled
		CMainSingleton::PostMaster().Send({ PostMaster::SMSG_ENABLE_CANVAS, nullptr });

		IRONWROUGHT->GetActiveScene().MainCamera()->Fade(true, 1.0f);

		//float previousDistance = 0.0f;
		GameObject().myTransform->Position(mySettings.mySpawnPosition);
		myMovementLocked = false;
		myDetachedPlayerHead = nullptr;
		//std::cout << __FUNCTION__ << " Attack event end." << std::endl;
		return;
	}

	float a = atan2f(GameObject().myTransform->Position().x - myDetachedPlayerHead->Position().x, GameObject().myTransform->Position().z - myDetachedPlayerHead->Position().z);
	Quaternion targetRotCamera = Quaternion::CreateFromYawPitchRoll(a, 0.0f, 0.0f);
	Quaternion rotCamera = myDetachedPlayerHead->Rotation();
	Quaternion slerp = Quaternion::Slerp(rotCamera, targetRotCamera, 7.0f * CTimer::Dt());
	myDetachedPlayerHead->Rotation(slerp);
	myPlayer->myTransform->Rotation(slerp);
}

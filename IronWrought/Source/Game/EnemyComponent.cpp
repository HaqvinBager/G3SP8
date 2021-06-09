#include "stdafx.h"
#include "EnemyComponent.h"
#include "CharacterController.h"
#include "AIController.h"
#include "TransformComponent.h"
#include "VFXSystemComponent.h"
#include <Scene.h>
#include "Engine.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"
#include "CapsuleColliderComponent.h"
#include "PatrolPointComponent.h"
#include "PlayerComponent.h"
#include "RigidBodyComponent.h"
#include "ModelComponent.h"
#include "PhysXWrapper.h"
#include "Engine.h"
#include "Scene.h"
#include <AStar.h>
#include "LineInstance.h"
#include "LineFactory.h"
#include <algorithm>
#include "Debug.h"
#include "PlayerControllerComponent.h"
#include "CameraComponent.h"

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings)
	: CComponent(aParent)
	, myPlayer(nullptr)
	, myCurrentState(EBehaviour::Idle)
	, myRigidBodyComponent(nullptr)
	, myMovementLocked(false)
	, myWakeUpTimer(0.f)
	, myIdlingTimer(0.0f)
	, myHasFoundPlayer(false)
	, myHasReachedAlertedTarget(true)
	, myHasReachedLastPlayerPosition(true)
	, myHeardSound(false)
	, myIsIdle(false)
	, mySqrdDistanceToPlayer(FLT_MAX)
	, myCloseToPlayerThreshold(FLT_MAX)
	, myAttackPlayerTimer(0.0f)
	, myAttackPlayerTimerMax(3.0f)
{
	//myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f, GameObject().myTransform, aHitReport);
	//myController->GetController().getActor()->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
	mySettings = someSettings;
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttackedPlayer, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::PropCollided, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyReachedTarget, this);
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyReachedLastPlayerPosition, this);
}

CEnemyComponent::~CEnemyComponent()
{
	myRigidBodyComponent = nullptr;
	for (size_t i = 0; i < myBehaviours.size(); ++i)
	{
		delete myBehaviours[i];
	}
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttackedPlayer, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::PropCollided, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyReachedTarget, this);
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyReachedLastPlayerPosition, this);
}

void CEnemyComponent::Awake()
{
}

void CEnemyComponent::Start()
{
	myPlayer = CEngine::GetInstance()->GetActiveScene().Player();
	CScene& scene = CEngine::GetInstance()->GetActiveScene();
	myNavMesh = scene.NavMesh();

	if (mySettings.myPatrolGameObjectIds.size() > 0) {
		for (int i = 0; i < mySettings.myPatrolGameObjectIds.size(); ++i) {
			CGameObject* patrolGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myPatrolGameObjectIds[i]);
			float points = mySettings.myPatrolIntrestValue[i];
			patrolGameObject->AddComponent<CPatrolPointComponent>(*patrolGameObject, points);
			scene.AddInstance(patrolGameObject->GetComponent<CPatrolPointComponent>());
		}
	}

	std::vector<CPatrolPointComponent*> patrolPositions;
	for (const int id : mySettings.myPatrolGameObjectIds) {
		CGameObject* patrolPointGameObject = scene.FindObjectWithID(id);
		patrolPositions.push_back(patrolPointGameObject->GetComponent<CPatrolPointComponent>());
	}
	myBehaviours.push_back(new CPatrol(patrolPositions, myNavMesh));

	CSeek* seekBehaviour = new CSeek(myNavMesh);
	myBehaviours.push_back(seekBehaviour);
	if (myPlayer != nullptr)
	{
		seekBehaviour->SetTarget(myPlayer->myTransform);
	}

	myBehaviours.push_back(new CAlerted(myNavMesh));
	myBehaviours.push_back(new CIdle());

	CAttack* attack = new CAttack();
	myBehaviours.push_back(attack);

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myRigidBodyComponent = GameObject().GetComponent<CRigidBodyComponent>();
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(mySettings.mySpeed);
	}

	mySpawnPosition = GameObject().myTransform->Position();
}

void CEnemyComponent::Update()//får bestämma vilket behaviour vi vill köra i denna Update()!!!
{
	if (myCurrentState == EBehaviour::Attack)
	{
		UpdateAttackEvent();
		return;
	}

	if (!myMovementLocked) {
		mySqrdDistanceToPlayer = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

		float range = 6.0f;
		myCloseToPlayerThreshold = range * 0.8f;
		//std::cout << __FUNCTION__ << " SqrDist: " << mySqrdDistanceToPlayer << " threshold: " << myCloseToPlayerThreshold << std::endl;
		Vector3 dir = GameObject().myTransform->Transform().Forward();
		Vector3 enemyPos = GameObject().myTransform->Position();
		Vector3 furthestLookingPoint = GameObject().myTransform->WorldPosition() + (GameObject().myTransform->Transform().Forward() * range);
		Vector3 furthestLookingPointToEnemy = furthestLookingPoint - enemyPos;
		Vector3 playerPos = myPlayer->myTransform->WorldPosition();
		Vector3 playerToEnemy = playerPos - enemyPos;
		float dot = furthestLookingPointToEnemy.Dot(playerToEnemy);
		float lengthSquaredEnemy = furthestLookingPointToEnemy.LengthSquared();
		float lengthSquaredPlayer = playerToEnemy.LengthSquared();
		float degrees = std::acos(dot / sqrt(lengthSquaredEnemy * lengthSquaredPlayer)) * 180.f / PI;
		float viewAngle = 65.f;

		if (degrees <= viewAngle) {
			Vector3 direction = playerPos - enemyPos;
			PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(enemyPos, direction, range, CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT | CPhysXWrapper::ELayerMask::PLAYER);
			if (hit.getNbAnyHits() > 0) {
				CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
				if (!transform && !myHasFoundPlayer) {
					if (myHasReachedLastPlayerPosition == true) {
						myIsIdle = true;
						SetState(EBehaviour::Idle);
					}
					else {
						myIsIdle = false;	
					}
					myHeardSound = false;
					myHasReachedAlertedTarget = true;
					myHasFoundPlayer = true;
					myHasReachedLastPlayerPosition = false;
					CMainSingleton::PostMaster().Send({ EMessageType::EnemyFoundPlayer });
				}
			}
		}
		else if (myHasFoundPlayer)
		{
			myHasFoundPlayer = false;
			SMessage msg;
			msg.data = static_cast<void*>(&playerPos);
			msg.myMessageType = EMessageType::EnemyLostPlayer;
			CMainSingleton::PostMaster().Send(msg);
			myIdlingTimer = 0.0f;
		}

		if (myIsIdle) {
			myIdlingTimer += CTimer::Dt();
			if (myIdlingTimer >= 0.5f) {
				myIdlingTimer = 0.0f;
				myIsIdle = false;
			}
		}
		else {

			if (mySqrdDistanceToPlayer <= 2.0f) {
				myHasFoundPlayer = false;
				myHeardSound = false;
				myIsIdle = false;
				myHasReachedAlertedTarget = true;
				myHasReachedLastPlayerPosition = true;
				SetState(EBehaviour::Attack);
			}
			else if (myHasFoundPlayer) {
				mySettings.mySpeed = 3.0f;
				SetState(EBehaviour::Seek);
			}
			else if (!myHasFoundPlayer && !myHasReachedLastPlayerPosition /*&& !myHeardSound && myHasReachedAlertedTarget*/) {
				mySettings.mySpeed = 3.0f;
				SetState(EBehaviour::Seek);
			}
			else if (!myHeardSound && !myHasFoundPlayer && myHasReachedLastPlayerPosition) {
				mySettings.mySpeed = 1.5f;
				SetState(EBehaviour::Patrol);
			}
			else if (myHasReachedAlertedTarget) {
				myHasReachedAlertedTarget = false;
			}
			else if (!myHasReachedAlertedTarget) {
				mySettings.mySpeed = 3.0f;
				SetState(EBehaviour::Alerted);
			}
		}

		Vector3 targetDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());
		targetDirection.y = 0;
		GameObject().myTransform->Move(targetDirection * mySettings.mySpeed * CTimer::Dt());
		float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
		myCurrentOrientation = Lerp(myCurrentOrientation, targetOrientation, 10.0f * CTimer::Dt());
		GameObject().myTransform->Rotation({ 0, DirectX::XMConvertToDegrees(myCurrentOrientation) + 180.f, 0 });

		//if(myCurrentState == EBehaviour::Seek)
			CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyUpdateCurrentState, this });
	}
	else {
		myWakeUpTimer += CTimer::Dt();
		if (myWakeUpTimer >= 1.f) {
			myMovementLocked = false;
			myWakeUpTimer = 0.f;
		}
	}
}

void CEnemyComponent::FixedUpdate()
{
	//myController->Move({ 0.0f, -0.098f, 0.0f }, 1.f);
}

void CEnemyComponent::SetState(EBehaviour aState)
{
	if (myCurrentState == aState)
		return;

	myCurrentState = aState;
	myBehaviours[static_cast<int>(myCurrentState)]->Enter(GameObject().myTransform->Position());
	EMessageType msgType = EMessageType::Count;
	switch (myCurrentState)
	{
	case EBehaviour::Patrol:
	{
		msgType = EMessageType::EnemyPatrolState;
	}break;

	case EBehaviour::Seek:
	{
		msgType = EMessageType::EnemySeekState;
	}break;

	case EBehaviour::Attack:
	{
		msgType = EMessageType::EnemyAttackState;
	}break;

	case EBehaviour::Alerted:
	{
		msgType = EMessageType::EnemyAlertedState;
	}break;

	case EBehaviour::Idle:
	{
		msgType = EMessageType::EnemyIdleState;
	}break;

	default:
		break;
	}
	if (msgType == EMessageType::Count)
		return;

	CMainSingleton::PostMaster().SendLate({ msgType, this });
}

const CEnemyComponent::EBehaviour CEnemyComponent::GetState() const
{
	return myCurrentState;
}

void CEnemyComponent::Receive(const SStringMessage& /*aMsg*/)
{
}

void CEnemyComponent::Receive(const SMessage& aMsg)
{
	if (aMsg.myMessageType == EMessageType::EnemyAttackedPlayer)
	{
		// Lock enemy movement
		// Player copies enemies rotation + rotate 180
		// Lock player movement for attack state time + some more
		// ? Rotate player so it better sees the enemy (i.e up or down)?
		// Set attack state timer to time of animation (2s?)
		// Start camera fade at half time
		// End of timer: enemy->spawn pos, fade camera in!

		myMovementLocked = true;
		bool lockCamera = true;
		CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
		CPlayerControllerComponent* plCtrl = myPlayer->GetComponent<CPlayerControllerComponent>();
		plCtrl->ForceStand();
		plCtrl->LockMovementFor(myAttackPlayerTimerMax + 0.75f);
		myPlayer->myTransform->CopyRotation(this->GameObject().myTransform->Transform());
		myPlayer->myTransform->Rotate({0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f});
		myPlayer->myTransform->FetchChildren()[0]->CopyRotation(myPlayer->myTransform->Transform()); // Camera rotates player, if not updated here camera will snap the player back to previous rotation on end of event.

		IRONWROUGHT->GetActiveScene().MainCamera()->SetTrauma(4.0f);

		myAttackPlayerTimer = myAttackPlayerTimerMax;
		return;
	}

	if (aMsg.myMessageType == EMessageType::PropCollided) {
		CGameObject* gameobject = reinterpret_cast<CGameObject*>(aMsg.data);
		if (gameobject) {
			std::vector<Vector3> path = myNavMesh->CalculatePath(GameObject().myTransform->Position(), gameobject->myTransform->Position(), myNavMesh);
			if (myNavMesh->PathLength(path, GameObject().myTransform->Position()) <= 20.f && !myHasFoundPlayer && myHasReachedLastPlayerPosition) {
				SetState(EBehaviour::Idle);
				myIsIdle = true;
				//play heardsound sound
				CAlerted* alertedBehaviour = static_cast<CAlerted*>(myBehaviours[static_cast<int>(EBehaviour::Alerted)]);
				if (alertedBehaviour) {
					alertedBehaviour->SetAlertedPosition(gameobject->myTransform->Position());
					myHasReachedAlertedTarget = false;
					myHeardSound = true;
				}
			}
		}
	}
	if (aMsg.myMessageType == EMessageType::EnemyReachedLastPlayerPosition) {
		//std::cout << " REACHED " << std::endl;
		myHasReachedLastPlayerPosition = true;
		myIsIdle = true;
		SetState(EBehaviour::Idle);
	}

	if (aMsg.myMessageType == EMessageType::EnemyReachedTarget) {
		myIsIdle = true;
		myHasReachedAlertedTarget = true;
		myHeardSound = false;
		SetState(EBehaviour::Idle);
	}
}

const float CEnemyComponent::PercentileDistanceToPlayer() const
{
	return mySqrdDistanceToPlayer / (myCloseToPlayerThreshold * myCloseToPlayerThreshold);
}

void CEnemyComponent::UpdateAttackEvent()
{
	myAttackPlayerTimer -= CTimer::Dt();
	if (myAttackPlayerTimer <= 0.0f)
	{
		CPlayerControllerComponent* plCtrl = myPlayer->GetComponent<CPlayerControllerComponent>();
		plCtrl->Crouch();
		//plCtrl->OnCrouch();
		bool lockCamera = false;
		CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
		IRONWROUGHT->GetActiveScene().MainCamera()->Fade(true, 0.75f);
		GameObject().myTransform->Position(mySpawnPosition);
		myMovementLocked = false;
		SetState(EBehaviour::Idle);
		std::cout << __FUNCTION__ << " Attack event end." << std::endl;
		return;
	}

	if (myAttackPlayerTimer <= myAttackPlayerTimerMax * 0.55f && myAttackPlayerTimer >= myAttackPlayerTimerMax * 0.5f)
	{
		// Fade out
		std::cout << __FUNCTION__ << " Camera fade out." << std::endl;
		IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, myAttackPlayerTimerMax * 0.4f, true);
		IRONWROUGHT->GetActiveScene().MainCamera()->SetTrauma(2.0f);
	}
}


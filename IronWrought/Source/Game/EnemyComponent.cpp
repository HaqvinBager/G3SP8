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

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings)
	: CComponent(aParent)
	, myPlayer(nullptr)
	, myCurrentState(EBehaviour::Count)
	, myRigidBodyComponent(nullptr)
	, myMovementLocked(false)
	, myWakeUpTimer(0.f)
	, myHasFoundPlayer(false)
{
	//myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f, GameObject().myTransform, aHitReport);
	//myController->GetController().getActor()->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
	mySettings = someSettings;
	CMainSingleton::PostMaster().Subscribe(EMessageType::EnemyAttackedPlayer, this);
}

CEnemyComponent::~CEnemyComponent()
{
	myRigidBodyComponent = nullptr;
	for (size_t i = 0; i < myBehaviours.size(); ++i)
	{
		delete myBehaviours[i];
	}
	CMainSingleton::PostMaster().Unsubscribe(EMessageType::EnemyAttackedPlayer, this);
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

			//CLineInstance* myLine2 = new CLineInstance();
			//myLine2->Init(CLineFactory::GetInstance()->CreateLine(GameObject().myTransform->Position(), patrolGameObject->myTransform->Position(), { 0,255,0,255 }));
			//CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);
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

	//CAttack* attack = new CAttack(this, myPatrolPositions[0]);
	/*if(myPlayer != nullptr)
		attack->SetTarget(myPlayer->myTransform);
	myBehaviours.push_back(attack);*/

	mySettings.mySpeed = mySettings.mySpeed < 5.0f ? 5.0f : mySettings.mySpeed;

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myRigidBodyComponent = GameObject().GetComponent<CRigidBodyComponent>();
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(mySettings.mySpeed);
	}
}

void CEnemyComponent::Update()//får bestämma vilket behaviour vi vill köra i denna Update()!!!
{
	if (!myMovementLocked) {
		//float distanceToPlayer = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

		float range = 10.f;
		Vector3 dir = GameObject().myTransform->Transform().Forward();
		//måste offseta mot origo då angle för två vectorers kollas mot origo i världen
		Vector3 enemyPos = GameObject().myTransform->Position();
		Vector3 furthestLookingPoint = GameObject().myTransform->WorldPosition() + (GameObject().myTransform->Transform().Forward() * range);
		Vector3 furthestLookingPointToEnemy = furthestLookingPoint - enemyPos;
		Vector3 playerPos = myPlayer->myTransform->WorldPosition();
		Vector3 playerToEnemy = playerPos - enemyPos;
		float dot = furthestLookingPointToEnemy.Dot(playerToEnemy);
		float lengthSquaredEnemy = furthestLookingPointToEnemy.LengthSquared();
		float lengthSquaredPlayer = playerToEnemy.LengthSquared();
		float degrees = std::acos(dot / sqrt(lengthSquaredEnemy * lengthSquaredPlayer)) * 180.f / PI;
		//std::cout << "Degrees: " << degrees << std::endl;
		float viewAngle = 60.f;
		myHasFoundPlayer = false;
		if (degrees <= viewAngle) {
			Vector3 direction = playerPos - enemyPos;
			PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(enemyPos, direction, range, CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT | CPhysXWrapper::ELayerMask::PLAYER);
			if (hit.getNbAnyHits() > 0) {
				CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
				if (!transform) {
					myHasFoundPlayer = true;
				}
			}
		}
		if (myHasFoundPlayer) {
			std::cout << "SEEK" << std::endl;
			SetState(EBehaviour::Seek);
		}
		else {
			std::cout << "PATROL" << std::endl;
			SetState(EBehaviour::Patrol);
		}

		//if (mySettings.myRadius * mySettings.myRadius >= distanceToPlayer) {
		//	/*if (distanceToPlayer <= mySettings.myAttackDistance * mySettings.myAttackDistance)
		//	{
		//		std::cout << "ATTACK" << std::endl;
		//		SetState(EBehaviour::Attack);
		//	}
		//	else
		//	{*/
		//		//std::cout << "SEEK" << std::endl;
		//		//SetState(EBehaviour::Seek);
		//	//}
		//}
		//else {
		//	//std::cout << "PATROL" << std::endl;
		//	SetState(EBehaviour::Patrol);
		//}

		if (myRigidBodyComponent) {
			Vector3 targetDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());

			targetDirection.y = 0;
			myRigidBodyComponent->AddForce(targetDirection, mySettings.mySpeed);
			float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
			myCurrentOrientation = Lerp(myCurrentOrientation, targetOrientation, 2.0f * CTimer::Dt());
			GameObject().myTransform->Rotation({ 0, DirectX::XMConvertToDegrees(myCurrentOrientation) + 180.f, 0 });
		}
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
		myMovementLocked = true;
	}
}

//CPatrolPointComponent* CEnemyComponent::FindBestPatrolPoint()
//{
//	const std::vector<CPatrolPointComponent*>& patrolPoints = CEngine::GetInstance()->GetActiveScene().PatrolPoints();
//	std::vector<float> intrestValues;
//	if (patrolPoints.size() > 0) {
//		for (int i = 0; i < patrolPoints.size(); ++i) {
//			Vector3 patrolPositions = patrolPoints[i]->GameObject().myTransform->Position();
//			Vector3 dist = patrolPositions - GameObject().myTransform->Position();
//			float length = dist.LengthSquared() / 10.f;
//			patrolPoints[i]->AddValue(length);
//			intrestValues.emplace_back(patrolPoints[i]->GetIntrestValue());
//			//std::cout << "[" << i << "] " << "Length Value: " << patrolPoints[i]->GetIntrestValue() << std::endl;
//		}
//
//		float min = *std::min_element(intrestValues.begin(), intrestValues.end());
//		//std::cout << "Length Value: " << min << std::endl;
//
//		for (int i = 0; i < patrolPoints.size(); ++i) {
//			if (patrolPoints[i]->GetIntrestValue() == min) {
//				return patrolPoints[i];
//			}
//		}
//	}
//	return nullptr;
//}

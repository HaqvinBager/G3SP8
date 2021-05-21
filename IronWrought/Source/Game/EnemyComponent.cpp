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
#include "PlayerComponent.h"
#include "RigidBodyComponent.h"
#include "ModelComponent.h"
#include "PhysXWrapper.h"
#include <AStar.h>

//EnemyComp

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings)
	: CComponent(aParent)
	, myPlayer(nullptr)
	, myEnemy(nullptr)
	, myCurrentState(EBehaviour::Count)
	, myRigidBodyComponent(nullptr)
	, myMovementLocked(false)
	, myWakeUpTimer(0.f)
{
	//myController = CEngine::GetInstance()->GetPhysx().CreateCharacterController(GameObject().myTransform->Position(), 0.6f * 0.5f, 1.8f * 0.5f, GameObject().myTransform, aHitReport);
	//myController->GetController().getActor()->setRigidBodyFlag(PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true);
	mySettings = someSettings;
	myPitch = 0.0f;
	myYaw = 0.0f;
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
	CScene &scene = CEngine::GetInstance()->GetActiveScene();
	myNavMesh = scene.NavMesh();
	
	for (const auto id : mySettings.myPatrolGameObjectIds) {
		CTransformComponent* patrolTransform = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(id)->myTransform;
		myPatrolPositions.push_back(patrolTransform->Position());
	}
	myBehaviours.push_back(new CPatrol(myPatrolPositions, myNavMesh));

	CSeek* seekBehaviour = new CSeek(myNavMesh);
	myBehaviours.push_back(seekBehaviour);
	if (myPlayer != nullptr)
	{
		seekBehaviour->SetTarget(myPlayer->myTransform);
	}

	if (myPatrolPositions.empty())
		myPatrolPositions.push_back({ 0.0f,0.0f,0.0f });

	CAttack* attack = new CAttack(this, myPatrolPositions[0]);
	if(myPlayer != nullptr)
		attack->SetTarget(myPlayer->myTransform);
	myBehaviours.push_back(attack);

	//this->GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(0);

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myRigidBodyComponent = GameObject().GetComponent<CRigidBodyComponent>();
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true); 
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(mySettings.mySpeed);
	}
	myCurrentHealth = mySettings.myHealth;
}

void CEnemyComponent::Update()//får bestämma vilket behaviour vi vill köra i denna Update()!!!
{
	if (!myMovementLocked) {
		float distanceToPlayer = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

		if (mySettings.myRadius * mySettings.myRadius >= distanceToPlayer) {
			if (distanceToPlayer <= mySettings.myAttackDistance * mySettings.myAttackDistance)
			{
				std::cout << "ATTACK" << std::endl;
				SetState(EBehaviour::Attack);
			}
			else
			{
				std::cout << "SEEK" << std::endl;
				SetState(EBehaviour::Seek);
			}
		}
		else {
			std::cout << "PATROL" << std::endl;
			SetState(EBehaviour::Patrol);
		}

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

	if (myCurrentHealth <= 0.f) {
		Dead();
	}
}

void CEnemyComponent::FixedUpdate()
{
	//myController->Move({ 0.0f, -0.098f, 0.0f }, 1.f);
}

void CEnemyComponent::TakeDamage(float aDamage)
{
	myCurrentHealth -= aDamage;
	CMainSingleton::PostMaster().SendLate({ EMessageType::EnemyTakeDamage, this });
}

void CEnemyComponent::SetState(EBehaviour aState)
{
	if (myCurrentState == aState)
		return;

	myCurrentState = aState;

	EMessageType msgType = EMessageType::Count;
	switch (myCurrentState)
	{
		case EBehaviour::Patrol:
		{
			//std::cout << __FUNCTION__ << " " << "PATROL" << std::endl;
			msgType = EMessageType::EnemyPatrolState;
		}break;

		case EBehaviour::Seek:
		{
			//std::cout << __FUNCTION__ << " " << "SEEK" << std::endl;
			msgType = EMessageType::EnemySeekState;
		}break;

		case EBehaviour::Attack:
		{
			//std::cout << __FUNCTION__ << " " << "ATTACK" << std::endl;
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

void CEnemyComponent::Dead()
{
	GameObject().Active(false);
}

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

CEnemyComponent::CEnemyComponent(CGameObject& aParent, const SEnemySetting& someSettings, SNavMesh* aNavMesh)
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
	, myAttackPlayerTimerMax(1.8f)
	, myNavMesh(aNavMesh)
	, myDetectionTimer(0.0f)
	, myHasScreamed(false)
	, myDetachedPlayerHead(nullptr)
	, myCurrentVignetteBlend(0.0f)
	, myTargetVignetteBlend(0.0f)

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
	myNavMesh = nullptr;
}

void CEnemyComponent::Awake()
{
}

void CEnemyComponent::Start()
{
	myPlayer = CEngine::GetInstance()->GetActiveScene().Player();
	CScene& scene = CEngine::GetInstance()->GetActiveScene();

#ifdef _DEBUG
	std::vector<DirectX::SimpleMath::Vector3> positions;
	UINT size = static_cast<UINT>(myNavMesh->myTriangles.size()) * 6;
	positions.reserve(size);

	for (UINT i = 0, j = 0; i < size && j < myNavMesh->myTriangles.size(); i += 6, j++)
	{
		positions.push_back(myNavMesh->myTriangles[j]->myVertexPositions[0]);
		positions.push_back(myNavMesh->myTriangles[j]->myVertexPositions[1]);
		positions.push_back(myNavMesh->myTriangles[j]->myVertexPositions[2]);
		positions.push_back(myNavMesh->myTriangles[j]->myVertexPositions[0]);
		positions.push_back(myNavMesh->myTriangles[j]->myVertexPositions[1]);
		positions.push_back(myNavMesh->myTriangles[j]->myVertexPositions[2]);
	}

	CLineInstance* navMeshGrid = new CLineInstance();
	navMeshGrid->Init(CLineFactory::GetInstance()->CreatePolygon(positions));
	scene.AddInstance(navMeshGrid);
#endif // _DEBUG

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
	myIdleState = new CIdle();
	myBehaviours.push_back(myIdleState);
	if (myPlayer != nullptr)
	{
		myIdleState->SetTarget(myPlayer->myTransform);
	}

	CAttack* attack = new CAttack();
	myBehaviours.push_back(attack);

	if (GameObject().GetComponent<CRigidBodyComponent>()) {
		myRigidBodyComponent = GameObject().GetComponent<CRigidBodyComponent>();
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
		myRigidBodyComponent->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(mySettings.mySpeed);
	}

	myBehaviours.push_back(new CDetection());

	mySpawnPosition = GameObject().myTransform->Position();
}

void CEnemyComponent::Update()//får bestämma vilket behaviour vi vill köra i denna Update()!!!
{
	if (myCurrentState == EBehaviour::Attack)
	{
		UpdateAttackEvent();
		return;
	}

	UpdateVignette();
	if (!myMovementLocked) {
		mySqrdDistanceToPlayer = Vector3::DistanceSquared(myPlayer->myTransform->Position(), GameObject().myTransform->Position());

		float range = 6.0f;
		myCloseToPlayerThreshold = range * 1.0f;
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
						if (myPlayer != nullptr)
						{
							myIdleState->SetTarget(myPlayer->myTransform);
						}
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
		else if (myHasFoundPlayer)//Out of View
		{
			myIdlingTimer = 0.0f;
			myDetectionTimer = 0.0f;
			myHasFoundPlayer = false;
			myHasReachedLastPlayerPosition = false;
			SMessage msg;
			msg.data = static_cast<void*>(&playerPos);
			msg.myMessageType = EMessageType::EnemyLostPlayer;
			CMainSingleton::PostMaster().Send(msg);
		}

		if (myIsIdle) {
			myIdlingTimer += CTimer::Dt();
			if (myIdlingTimer >= 0.5f) {
				myIdlingTimer = 0.0f;
				myIsIdle = false;
			}
		}
		else {

			// is compared to sqrd distance => != 3m
			if ((mySqrdDistanceToPlayer <= myGrabRange && myHasFoundPlayer) || mySqrdDistanceToPlayer <= (myGrabRange * 0.6f)) {
				myHasFoundPlayer = false;
				myHeardSound = false;
				myIsIdle = false;
				myHasReachedAlertedTarget = true;
				myHasReachedLastPlayerPosition = true;
				SetState(EBehaviour::Attack);
				return;
			}
			else if (myHasFoundPlayer) {

				if (myCurrentState != EBehaviour::Detection && myCurrentState != EBehaviour::Seek)
					SetState(EBehaviour::Detection);
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
				//mySettings.mySpeed = 3.0f;
				//SetState(EBehaviour::Alerted);
			}
		}

		Vector3 targetDirection = myBehaviours[static_cast<int>(myCurrentState)]->Update(GameObject().myTransform->Position());
		targetDirection.y = 0;
		if (myCurrentState != EBehaviour::Idle) {
			GameObject().myTransform->Move(targetDirection * mySettings.mySpeed * CTimer::Dt());
		}

		float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
		myCurrentOrientation = Lerp(myCurrentOrientation, targetOrientation, 10.0f * CTimer::Dt());
		GameObject().myTransform->Rotation({ 0, myCurrentOrientation + 180.f, 0 });

		switch (myCurrentState)
		{
		case EBehaviour::Seek:
		{
			myCurrentStateBlend = PercentileDistanceToPlayer();
		}break;

		case EBehaviour::Alerted:
		{
			CAlerted* alertedBehaviour = static_cast<CAlerted*>(myBehaviours[static_cast<int>(EBehaviour::Alerted)]);
			myCurrentStateBlend = alertedBehaviour->PercentileAlertedTimer();
		}break;

		case EBehaviour::Detection:
		{
			
			CDetection* detectedBehaviour = static_cast<CDetection*>(myBehaviours[static_cast<int>(EBehaviour::Detection)]);
			myCurrentStateBlend = detectedBehaviour->PercentileOfTimer();
			if (myCurrentStateBlend <= 0.0f)
			{
				mySettings.mySpeed = 3.0f;
				SetState(EBehaviour::Seek);
				myCurrentStateBlend = 1.0f;
			}
		}break;

		default:
		{
			myCurrentStateBlend = 0.0f;
		}break;
		}

		CMainSingleton::PostMaster().Send({ EMessageType::EnemyUpdateCurrentState, this });
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
	bool aggro = false;
	switch (myCurrentState)
	{
	case EBehaviour::Patrol:
	{
		aggro = false;
		CMainSingleton::PostMaster().Send({ EMessageType::EnemyAggro, &aggro });
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

	case EBehaviour::Detection:
	{
		aggro = true;
		CMainSingleton::PostMaster().Send({EMessageType::EnemyAggro, &aggro} );
		/*CMainSingleton::PostMaster().Send({ EMessageType::EnemyFoundPlayerScream });*/
		msgType = EMessageType::EnemyDetectionState;
	}break;

	default:
		break;
	}
	if (msgType == EMessageType::Count)
		return;

	CMainSingleton::PostMaster().Send({ msgType, this });
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
		//std::cout << __FUNCTION__ << " ATTACK PLAYER " << std::endl;
		myMovementLocked = true;
		bool lockCamera = true;
		CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
		CPlayerControllerComponent* plCtrl = myPlayer->GetComponent<CPlayerControllerComponent>();
		plCtrl->ForceStand();
		plCtrl->LockMovementFor(myAttackPlayerTimerMax + 0.75f);
		//CMainSingleton::PostMaster().Send({ PostMaster::SMSG_DISABLE_GLOVE, nullptr });// Doing this did not work out well
		CMainSingleton::PostMaster().Send({ PostMaster::SMSG_DISABLE_CANVAS, nullptr });

		Vector3 targetDirection = myPlayer->myTransform->Position() - this->GameObject().myTransform->Position();
		targetDirection.Normalize();
		float targetOrientation = WrapAngle(atan2f(targetDirection.x, targetDirection.z));
		GameObject().myTransform->Rotation({ 0, targetOrientation + 180.f, 0 });

		// Detach player head
		myDetachedPlayerHead = myPlayer->myTransform->FetchChildren()[0];
		myDetachedPlayerHead->RemoveParent();
		//float a = atan2f(GameObject().myTransform->Position().x - myDetachedPlayerHead->Position().x, GameObject().myTransform->Position().z - myDetachedPlayerHead->Position().z);
		//myDetachedPlayerHead->Rotation({ 0.f, DirectX::XMConvertToDegrees(a), 0.f });

		myAttackPlayerTimer = myAttackPlayerTimerMax;
		IRONWROUGHT->GetActiveScene().MainCamera()->Fade(false, myAttackPlayerTimerMax + 1.f, true);
		return;
	}

	if (aMsg.myMessageType == EMessageType::PropCollided) {
		
		if (myDetachedPlayerHead)// Return if the enemy is attacking the players head.
			return;
		
		CGameObject* gameobject = reinterpret_cast<CGameObject*>(aMsg.data);
		if (gameobject) {
			std::vector<Vector3> path = myNavMesh->CalculatePath(GameObject().myTransform->Position(), gameobject->myTransform->Position(), myNavMesh);
			if (myNavMesh->PathLength(path, GameObject().myTransform->Position()) <= 20.f && !myHasFoundPlayer && myHasReachedLastPlayerPosition) {
				/*SetState(EBehaviour::Idle);*/
				//myIsIdle = true;
				mySettings.mySpeed = 3.0f;
				//play heardsound sound
				CAlerted* alertedBehaviour = static_cast<CAlerted*>(myBehaviours[static_cast<int>(EBehaviour::Alerted)]);
				if (alertedBehaviour) {
					alertedBehaviour->SetAlertedPosition(gameobject->myTransform->Position());
					myHasReachedAlertedTarget = false;
					myHeardSound = true;
				}
				SetState(EBehaviour::Alerted);
			}
		}
		return;
	}

	if (aMsg.myMessageType == EMessageType::EnemyReachedLastPlayerPosition) {
		//std::cout << " REACHED " << std::endl;
		
		myHasReachedLastPlayerPosition = true;
		myIsIdle = true;
		SetState(EBehaviour::Idle);
		return;
	}

	if (aMsg.myMessageType == EMessageType::EnemyReachedTarget) {
		myIsIdle = true;
		myHasReachedAlertedTarget = true;
		myHeardSound = false;
		SetState(EBehaviour::Idle);
		return;
	}
}

const float CEnemyComponent::PercentileDistanceToPlayer() const
{
	return mySqrdDistanceToPlayer / (myCloseToPlayerThreshold * myCloseToPlayerThreshold);
}

const float CEnemyComponent::CurrentStateBlendValue() const
{
	return myCurrentStateBlend;
}

void CEnemyComponent::UpdateAttackEvent()
{
	//std::cout << __FUNCTION__ << "." << std::endl;
	UpdateVignette();
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

		float previousDistance = 0.0f;
		if (!mySettings.mySpawnPoints.empty())
		{
			for (auto& spawnPosition : mySettings.mySpawnPoints)
			{
				float distance = Vector3::Distance(GameObject().myTransform->Position(), spawnPosition);
				if (distance > previousDistance)
				{
					mySpawnPosition = { spawnPosition.x, GameObject().myTransform->Position().y, spawnPosition.z };
				}
				previousDistance = distance;
			}
		}
		GameObject().myTransform->Position(mySpawnPosition);
		myMovementLocked = false;
		myDetachedPlayerHead = nullptr;
		SetState(EBehaviour::Idle);
		//std::cout << __FUNCTION__ << " Attack event end." << std::endl;
		UpdateVignette(0.0f);
		return;
	}

	float a = atan2f(GameObject().myTransform->Position().x - myDetachedPlayerHead->Position().x, GameObject().myTransform->Position().z - myDetachedPlayerHead->Position().z);
	Quaternion targetRotCamera = Quaternion::CreateFromYawPitchRoll(a, 0.0f, 0.0f);
	Quaternion rotCamera = myDetachedPlayerHead->Rotation();
	Quaternion slerp = Quaternion::Slerp(rotCamera, targetRotCamera, 7.0f * CTimer::Dt());
	myDetachedPlayerHead->Rotation(slerp);
	myPlayer->myTransform->Rotation(slerp);
}

float CEnemyComponent::SmoothStep(float a, float b, float t)
{
	float x = std::clamp((t - a) / (b - a), 0.0f, 1.0f);
	return x * x * (3.0f - 2.0f * x);
}

float CEnemyComponent::InverseLerp(float a, float b, float v)
{
	return (v - a) / (b - a);
}

#include "CameraComponent.h"
void CEnemyComponent::UpdateVignette(const float aDotOverload)
{
	if (aDotOverload >= 0.0f)
	{
		CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
		float normalizedBlend = SmoothStep(0.0f, 1.0f, aDotOverload);

		data.myVignetteStrength = Lerp(0.35f, 5.0f, normalizedBlend);
		CEngine::GetInstance()->SetPostProcessingBufferData(data);
		IRONWROUGHT_ACTIVE_SCENE.MainCamera()->SetTrauma(normalizedBlend, CCameraComponent::ECameraShakeState::EnemySway);

		//std::cout << __FUNCTION__ << " update overload" << std::endl;
		return;
	}

	float range = 100.0f;
	Vector3 enemyPos = GameObject().myTransform->Position();
	Vector3 playerPos = myPlayer->myTransform->WorldPosition();
	enemyPos.y = playerPos.y;

	Vector3 direction = playerPos - enemyPos;
	direction.Normalize();
	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(enemyPos, direction, range, CPhysXWrapper::ELayerMask::STATIC_ENVIRONMENT | CPhysXWrapper::ELayerMask::PLAYER);
	if (hit.getNbAnyHits() > 0) {
		CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
		if (!transform)
		{
			float dot = myPlayer->myTransform->GetLocalMatrix().Forward().Dot(direction);

			//std::cout << "Enemy pos " << enemyPos.x << " " << enemyPos.y << " " <<  enemyPos.z << std::endl;
			//std::cout << "Player Pos " << playerPos.x << " " << playerPos.y << " " <<  playerPos.z << std::endl;

			//std::cout << __FUNCTION__ << " Dot: " << dot << std::endl;
			dot = InverseLerp(0.8f, 1.0f, dot);
			dot = std::clamp(dot, 0.0f, 1.0f);
			dot *= dot * dot;

			float timeVariationAmplitude = 0.1f;
			timeVariationAmplitude *= dot;
			float timeVariationSpeed = 10.0f;
			float timeVariation = abs(sinf(CTimer::Time() * timeVariationSpeed)) * timeVariationAmplitude * -1.0f;
			dot += timeVariation;

			CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
			float normalizedBlend = SmoothStep(0.0f, 1.0f, dot);

			myTargetVignetteBlend = normalizedBlend;

			/*myCurrentVignetteBlend = Lerp(0.0f, 1.0f, CTimer::Dt() * normalizedBlend);*/
			myCurrentVignetteBlend += CTimer::Dt() * 0.5f;
			if (myCurrentVignetteBlend > myTargetVignetteBlend)
				myCurrentVignetteBlend = myTargetVignetteBlend;

			data.myVignetteStrength = Lerp(0.35f, 3.0f, myCurrentVignetteBlend);
			CEngine::GetInstance()->SetPostProcessingBufferData(data);

			IRONWROUGHT_ACTIVE_SCENE.MainCamera()->SetTrauma(myCurrentVignetteBlend, CCameraComponent::ECameraShakeState::EnemySway);

			//std::cout << __FUNCTION__ << " Current: " << myCurrentVignetteBlend << std::endl;
		}
		else {
			CFullscreenRenderer::SPostProcessingBufferData data = CEngine::GetInstance()->GetPostProcessingBufferData();
			myCurrentVignetteBlend -= CTimer::Dt() * 0.5f;
			if (myCurrentVignetteBlend <= 0.0f)
				myCurrentVignetteBlend = 0.0f;
			float normalizedBlend = myCurrentVignetteBlend;

			data.myVignetteStrength = Lerp(0.35f, 3.0f, normalizedBlend);
			CEngine::GetInstance()->SetPostProcessingBufferData(data);

			IRONWROUGHT_ACTIVE_SCENE.MainCamera()->SetTrauma(normalizedBlend, CCameraComponent::ECameraShakeState::IdleSway);
		
			//std::cout << __FUNCTION__ << " update no hit" << std::endl;
		}
	}
}


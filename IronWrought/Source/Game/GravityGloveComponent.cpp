#include "stdafx.h"
#include "GravityGloveComponent.h"
#include "RigidBodyComponent.h"
#include "TransformComponent.h"
#include "VFXSystemComponent.h"
#include "PhysXWrapper.h"
#include "LineFactory.h"
#include "LineInstance.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidDynamicBody.h"
#include "PlayerControllerComponent.h"
#include "EnemyComponent.h"
#include "PlayerComponent.h"
#include "HealthPickupComponent.h"
#include "CharacterController.h"
#include "CameraComponent.h"
#include "BoxColliderComponent.h"

CGravityGloveComponent::CGravityGloveComponent(CGameObject& aParent, CTransformComponent* aGravitySlot)
	: CBehavior(aParent)
	, myGravitySlot(aGravitySlot)
	, myRigidStatic(nullptr)
	, myHoldingAItem(false)
{
	mySettings.myPushForce = 10.f;
	//mySettings.myDistanceToMaxLinearVelocity = 2.5f;
	mySettings.myMaxPushForce = 100.0f;
	mySettings.myMinPushForce = 10.0f;
	mySettings.myMinPullForce = 200.0f;

	mySettings.myMaxDistance = 0.75f;
	mySettings.myCurrentDistanceInverseLerp = 0.0f;
	myJoint = nullptr;
}

CGravityGloveComponent::~CGravityGloveComponent()
{
	myGravitySlot = nullptr;
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_ENABLE_GLOVE, this);
}

void CGravityGloveComponent::Awake()
{
	myRigidStatic = CEngine::GetInstance()->GetPhysx().GetPhysics()->createRigidStatic({ myGravitySlot->GetWorldMatrix().Translation().x,myGravitySlot->GetWorldMatrix().Translation().y, myGravitySlot->GetWorldMatrix().Translation().z });
	myRigidStatic->userData = (void*)GameObject().myTransform;
	CEngine::GetInstance()->GetPhysx().GetPXScene()->addActor(*myRigidStatic);

	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_GLOVE, this);
}

void CGravityGloveComponent::Start()
{
	//PostMaster::SCrossHairData data; // Wind down
	//data.myIndex = 0;
	//data.myShouldBeReversed = true;
	CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &myCrosshairData });
}

void CGravityGloveComponent::Update()
{
	// Use one at a time:
	InteractionLogicContinuous();
	//InteractionLogicOnInput();
	myRigidStatic->setGlobalPose(CEngine::GetInstance()->GetPhysx().ConvertToPxTransform(GameObject().myTransform->GetWorldMatrix()));
	if (myCurrentTarget.myRigidBodyPtr != nullptr)
	{
		if (myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().isSleeping()) {
			myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().wakeUp();
		}
		myHoldingAItem = true;
		myCurrentTarget.myRigidBodyPtr->IsHeld(true);
		//myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidBodyFlag(PxRigidBodyFlag::eRETAIN_ACCELERATIONS, false);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);
		if (myJoint == nullptr) {
			myJoint = CreateD6Joint(myRigidStatic, &myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody(), mySettings.myMaxDistance);
		}
		else {

			float dist = Vector3::Distance(GameObject().myTransform->WorldPosition(), myCurrentTarget.myRigidBodyPtr->GameObject().myTransform->Position());
			if (dist >= mySettings.myMaxDistance + 3.f) {
				myJoint->release();
				myJoint = nullptr;
				myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
				myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
				myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, false);
				myCurrentTarget.myRigidBodyPtr = nullptr;
				myHoldingAItem = false;
			}
		}
	}
	else {
		myHoldingAItem = false;
		if (myJoint) {
			myJoint->release();
		}
		myJoint = nullptr;
	}
}

void CGravityGloveComponent::Pull()
{
	Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
	Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();

	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance, CPhysXWrapper::ELayerMask::WORLD);

	//std::vector<CGameObject*> gameobjects = CEngine::GetInstance()->GetActiveScene().ActiveGameObjects();

	/*for (int i = 0; i < gameobjects.size(); ++i) {
		if (gameobjects[i]->GetComponent<CRigidBodyComponent>()) {
			Vector3 pos = gameobjects[i]->myTransform->Position();
			pos -= start;
			pos.Normalize();
			dir.Normalize();
			float lookPrecentage = dir.Dot(pos);
			std::cout << i << ": " << lookPrecentage << std::endl;
			if (lookPrecentage > 0.99f && lookPrecentage > 0.f) {
				myCurrentTarget = gameobjects[i]->GetComponent<CRigidBodyComponent>();
			}
		}
	}*/
	if (hit.getNbAnyHits() > 0)
	{
		CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
		if (transform == nullptr || transform->GetComponent<CEnemyComponent>())
		{
			return;
		}

		CRigidBodyComponent* rigidbody = nullptr;
		if (transform->GameObject().TryGetComponent<CRigidBodyComponent>(&rigidbody))
		{
			if (!rigidbody->IsKinematic()) {
				myCurrentTarget.Clear();

				myCurrentTarget.myRigidBodyPtr = rigidbody;
				myCurrentTarget.initialDistanceSquared = Vector3::DistanceSquared(myGravitySlot->WorldPosition(), transform->WorldPosition());

				CMainSingleton::PostMaster().Send({ EMessageType::GravityGlovePull, nullptr });
				//GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(0);
			}
		}

		//myCurrentTarget = transform->GetComponent<CRigidBodyComponent>();


	#ifdef _DEBUG
	//	CLineInstance* myLine = new CLineInstance();
	//	CLineInstance* myLine2 = new CLineInstance();
	//	myLine->Init(CLineFactory::GetInstance()->CreateLine(start, { hit.getAnyHit(0).position.x, hit.getAnyHit(0).position.y, hit.getAnyHit(0).position.z }, { 0,255,0,255 }));
	//	myLine2->Init(CLineFactory::GetInstance()->CreateLine(start, start + (dir * 5.f), { 255,0,0,255 }));
	//	CEngine::GetInstance()->GetActiveScene().AddInstance(myLine);
	//	CEngine::GetInstance()->GetActiveScene().AddInstance(myLine2);
	#endif
	}
	//myCurrentTarget->SetPosition(myGravitySlot->WorldPosition());
}

void CGravityGloveComponent::Pull(CTransformComponent* aTransform, CRigidBodyComponent* aRigidBodyTarget)
{
	if (aTransform == nullptr || aTransform->GetComponent<CEnemyComponent>() && aRigidBodyTarget == nullptr)
	{
		return;
	}

	if (!aRigidBodyTarget->IsKinematic()) {
		myCurrentTarget.Clear();
		aRigidBodyTarget->GetDynamicRigidBody()->GetBody().clearForce();
		aRigidBodyTarget->SetLinearVelocity({0.f, 0.f, 0.f});
		aRigidBodyTarget->SetAngularVelocity({0.f, 0.f, 0.f});
		aRigidBodyTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(10.f);
		myCurrentTarget.myRigidBodyPtr = aRigidBodyTarget;
		myCurrentTarget.initialDistanceSquared = Vector3::DistanceSquared(myGravitySlot->WorldPosition(), aTransform->WorldPosition());

		CMainSingleton::PostMaster().Send({ EMessageType::GravityGlovePull, nullptr });
	}
}

void CGravityGloveComponent::Release()
{
	if (myCurrentTarget.myRigidBodyPtr != nullptr)
	{
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, false);
		myCurrentTarget.myRigidBodyPtr = nullptr;
		bool released = true;
		CMainSingleton::PostMaster().Send({ EMessageType::GravityGlovePull, &released });
	}
}

void CGravityGloveComponent::Push()
{
	bool sendPushMessage = false;

	if (myCurrentTarget.myRigidBodyPtr != nullptr) {
		IRONWROUGHT->GetActiveScene().MainCamera()->SetTrauma(0.25f); // plz enable camera movement without moving player for shake??? ::)) Nico 2021-04-09

		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget.myRigidBodyPtr->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * myCurrentTarget.myRigidBodyPtr->GetMass(), EForceMode::EImpulse);
		myCurrentTarget.myRigidBodyPtr = nullptr;
		//GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(1);
		sendPushMessage = true;

	} else {
		Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
		Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();
		PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance, CPhysXWrapper::ELayerMask::WORLD | CPhysXWrapper::ELayerMask::ENEMY);
		if (hit.getNbAnyHits() > 0)
		{
			CTransformComponent* transform = (CTransformComponent*)hit.getAnyHit(0).actor->userData;
			if (transform == nullptr)
				return;

			CRigidBodyComponent* target = transform->GetComponent<CRigidBodyComponent>();
			if (target != nullptr)
			{
				if (!target->IsKinematic())
				{
					target->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * target->GetMass(), EForceMode::EImpulse);
					//GameObject().GetComponent<CVFXSystemComponent>()->EnableEffect(1);

					sendPushMessage = true;
				}
			}
		}
	}

	if (sendPushMessage)
	{
		CMainSingleton::PostMaster().SendLate({ EMessageType::GravityGlovePush, nullptr });
	}
}

void CGravityGloveComponent::Push(CTransformComponent* aTransform, CRigidBodyComponent* aRigidBodyTarget)
{
	bool sendPushMessage = false;

	if (myCurrentTarget.myRigidBodyPtr != nullptr) {
		IRONWROUGHT->GetActiveScene().MainCamera()->SetTrauma(0.25f); // plz enable camera movement without moving player for shake??? ::)) Nico 2021-04-09

		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget.myRigidBodyPtr->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * myCurrentTarget.myRigidBodyPtr->GetMass(), EForceMode::EImpulse);
		myCurrentTarget.myRigidBodyPtr = nullptr;
		sendPushMessage = true;

	} else {
		if (aTransform == nullptr)
			return;

		if (aRigidBodyTarget != nullptr)
		{
			if (!aRigidBodyTarget->IsKinematic())
			{
				aRigidBodyTarget->AddForce(-GameObject().myTransform->GetWorldMatrix().Forward(), mySettings.myPushForce * aRigidBodyTarget->GetMass(), EForceMode::EImpulse);

				sendPushMessage = true;
			}
		}
	}

	if (sendPushMessage)
	{
		CMainSingleton::PostMaster().SendLate({ EMessageType::GravityGlovePush, nullptr });
	}
}

void CGravityGloveComponent::OnEnable()
{
}

void CGravityGloveComponent::OnDisable()
{
}

void CGravityGloveComponent::Receive(const SStringMessage& aMessage)
{
	if (PostMaster::DisableGravityGlove(aMessage.myMessageType))
	{
		this->Enabled(false);
	}
	if (PostMaster::EnableGravityGlove(aMessage.myMessageType))
	{
		this->Enabled(true);
	}
}

void CGravityGloveComponent::InteractionLogicContinuous()
{
	// -4FPS! with continuous raycasts :S (compared to InteractionLogicOnInput() in Cottage) // Aki 2021 05 26
	Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
	Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();

	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance + 2.f, CPhysXWrapper::ELayerMask::DYNAMIC_OBJECTS);// ELayerMask could be changed to ::DYNAMIC only? // Aki 2021 05 26
	if (hit.getNbAnyHits() > 0)
	{
		CTransformComponent* transform = static_cast<CTransformComponent*>(hit.getAnyHit(0).actor->userData);
		if (transform != nullptr)
		{
			CRigidBodyComponent* rigidbody = nullptr;
			if (transform->GameObject().TryGetComponent<CRigidBodyComponent>(&rigidbody))
			{
				if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
				{
					Pull(transform, rigidbody);
				} 
			}

			if (myCurrentTarget.myRigidBodyPtr)
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::Holding;
			else
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::Targeted;
		}	
		else
		{
			myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::None;
		}
	}
	else
	{
		myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::None;
	}
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Right))
	{
		// Wind down
		//data.myIndex = 0;
		//data.myShouldBeReversed = true;
		if (myCurrentTarget.myRigidBodyPtr) {
			Push(myCurrentTarget.myRigidBodyPtr->GameObject().myTransform, myCurrentTarget.myRigidBodyPtr);
		}
	}
	if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
	{
		Release();
	}
	CMainSingleton::PostMaster().SendLate({ EMessageType::UpdateCrosshair, &myCrosshairData });
}

void CGravityGloveComponent::InteractionLogicOnInput()
{
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Right))
	{
		//PostMaster::SCrossHairData data; // Wind down
		//data.myIndex = 0;
		//data.myShouldBeReversed = true;
		//CMainSingleton::PostMaster().Send({ EMessageType::UpdateCrosshair, &data });
	
		Push();
	}
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		Pull();
	}
	else if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
	{
		Release();
	}
}

physx::PxD6Joint* CGravityGloveComponent::CreateD6Joint(PxRigidActor* actor0, PxRigidActor* actor1, float aOffest)
{
	PxTransform transform0 = PxTransform(0,0,aOffest);
	if (actor0->is<PxRigidDynamic>()) {
		transform0 = actor0->is<PxRigidDynamic>()->getCMassLocalPose();
	}
	
	PxTransform transform1 = PxTransform(PxIDENTITY());
	if (actor1->is<PxRigidDynamic>()) {
		transform1 = actor1->is<PxRigidDynamic>()->getCMassLocalPose();
	}

	PxD6Joint* d6Joint = PxD6JointCreate(CEngine::GetInstance()->GetActiveScene().PXScene()->getPhysics(), actor0, transform0, actor1, transform1);
	d6Joint->setMotion(PxD6Axis::eX, PxD6Motion::eLOCKED);
	d6Joint->setMotion(PxD6Axis::eY, PxD6Motion::eLOCKED);
	d6Joint->setMotion(PxD6Axis::eZ, PxD6Motion::eLOCKED);
	d6Joint->setLinearLimit(PxJointLinearLimit(1.0f, PxSpring(1.f, 1.f)));

	d6Joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eLIMITED);
	d6Joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eLIMITED);
	d6Joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eLIMITED);

	PxD6JointDrive drive(100.f, 15.f, PX_MAX_F32, false);
	d6Joint->setDrive(PxD6Drive::eX, drive);
	d6Joint->setDrive(PxD6Drive::eY, drive);
	d6Joint->setDrive(PxD6Drive::eZ, drive);
	d6Joint->setDrivePosition(PxTransform(0,0,0));
	d6Joint->setDriveVelocity(PxVec3(0,0,0), PxVec3(0,0,0));

	return d6Joint;
}

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
	, myExtendedOffsetArm(0.f)
	, myIsRotatingmode(false)
{
	mySettings.myPushForce = 10.f;
	//mySettings.myDistanceToMaxLinearVelocity = 2.5f;
	mySettings.myMaxPushForce = 100.0f;
	mySettings.myMinPushForce = 10.0f;
	mySettings.myMinPullForce = 200.0f;

	mySettings.myMaxDistance = 2.75f;
	mySettings.myMinDistance = 0.75f;
	mySettings.myStuckRange = 1.f;
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
		myCurrentTarget.myRigidBodyPtr->IsHeld(true);
		
		if (myIsRotatingmode) {
			RotateObject();
		}

		if (myJoint == nullptr) {
			myJoint = CreateD6Joint(myRigidStatic, &myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody(), mySettings.myMinDistance + myExtendedOffsetArm);
			myCurrentTarget.myRigidBodyPtr->LockAngular(true);
		}
		else {
			myJoint->setLocalPose(PxJointActorIndex::eACTOR0, PxTransform(0, 0, mySettings.myMinDistance + myExtendedOffsetArm));
			float dist = Vector3::Distance(GameObject().myTransform->WorldPosition(), myCurrentTarget.myRigidBodyPtr->GameObject().myTransform->Position());
			
			if (dist >= mySettings.myMinDistance + mySettings.myStuckRange + myExtendedOffsetArm) {
				myJoint->release();
				myJoint = nullptr; 
				myCurrentTarget.myRigidBodyPtr->LockAngular(false);
				myHoldingAItem = false;
				myCurrentTarget.myRigidBodyPtr = nullptr;
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

		myHoldingAItem = true;
		float dist = Vector3::Distance(GameObject().myTransform->WorldPosition(), myCurrentTarget.myRigidBodyPtr->GameObject().myTransform->Position());
		if (dist > (mySettings.myMaxDistance)) {
			myExtendedOffsetArm = 2.f;
		}
		else if (dist < mySettings.myMinDistance) {
			myExtendedOffsetArm = 0.f;
		}
		else {
			myExtendedOffsetArm = dist - mySettings.myMinDistance;
		}
		myCurrentTarget.initialDistanceSquared = Vector3::DistanceSquared(myGravitySlot->WorldPosition(), aTransform->WorldPosition());

		CMainSingleton::PostMaster().Send({ EMessageType::GravityGlovePull, nullptr });
	}
}

void CGravityGloveComponent::Release()
{
	if (myCurrentTarget.myRigidBodyPtr != nullptr)
	{
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(100.f);
		myCurrentTarget.myRigidBodyPtr->LockAngular(false);
		myCurrentTarget.myRigidBodyPtr = nullptr;
		if (myIsRotatingmode) {
			CEngine::GetInstance()->GetWindowHandler()->HidLockCursor(true);
			myIsRotatingmode = false;
		}
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
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, false);
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
// The following code was a test for when the enemy grabs the player while it is holding an item.
	//if (myCurrentTarget.myRigidBodyPtr)
	//{
	//	Push(myCurrentTarget.myRigidBodyPtr->GameObject().myTransform, myCurrentTarget.myRigidBodyPtr);
	//	//Release();
	//}
	//myHoldingAItem = false;
	//
	//myCurrentTarget.myRigidBodyPtr = nullptr;
	//myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::None;
	//CMainSingleton::PostMaster().SendLate({ EMessageType::UpdateCrosshair, &myCrosshairData });
}

void CGravityGloveComponent::Receive(const SStringMessage& aMessage)
{
	if (PostMaster::DisableGravityGlove(aMessage.myMessageType))
	{
		this->Enabled(false);
		return;
	}
	if (PostMaster::EnableGravityGlove(aMessage.myMessageType))
	{
		this->Enabled(true);
		return;
	}
}

void CGravityGloveComponent::InteractionLogicContinuous()
{
	// -4FPS! with continuous raycasts :S (compared to InteractionLogicOnInput() in Cottage) // Aki 2021 05 26
	Vector3 start = GameObject().myTransform->GetWorldMatrix().Translation();
	Vector3 dir = -GameObject().myTransform->GetWorldMatrix().Forward();

	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance, CPhysXWrapper::ELayerMask::WORLD);// ELayerMask could be changed to ::DYNAMIC only? // Aki 2021 05 26
	if (hit.getNbAnyHits() > 0)
	{
		CTransformComponent* transform = static_cast<CTransformComponent*>(hit.getAnyHit(0).actor->userData);
		if (transform != nullptr)
		{
			CRigidBodyComponent* rigidbody = nullptr;
			if (transform->GameObject().TryGetComponent<CRigidBodyComponent>(&rigidbody))
			{
				if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left) && myIsRotatingmode == false)
				{
					Pull(transform, rigidbody);
				} 
			}

			if (myCurrentTarget.myRigidBodyPtr)
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::Holding;
			else if(rigidbody || transform->GameObject().Tag() == "Painting")
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::Targeted;
			else
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::None;

		}	
		else
		{
			if (myCurrentTarget.myRigidBodyPtr)
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::Holding;
			else
				myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::None;
		}
	}
	else
	{
		if (myCurrentTarget.myRigidBodyPtr)
			myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::Holding;
		else
			myCrosshairData.myTargetStatus = PostMaster::SCrossHairData::ETargetStatus::None;
	}
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Right) && myIsRotatingmode == false)
	{
		if (myCurrentTarget.myRigidBodyPtr) {
			Push(myCurrentTarget.myRigidBodyPtr->GameObject().myTransform, myCurrentTarget.myRigidBodyPtr);
		}
	}
	if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
	{
		Release();
	}

	if (myHoldingAItem) {
		if (Input::GetInstance()->MouseWheel() < 0) {
			if (myExtendedOffsetArm <= 0.f) {
				myExtendedOffsetArm = 0.f;
			}
			else {
				myExtendedOffsetArm -= CTimer::Dt() * 10.f;
			}
		}
		else if (Input::GetInstance()->MouseWheel() > 0) {
			if (myExtendedOffsetArm >= 2.f) {
				myExtendedOffsetArm = 2.f;
			}
			else {
				myExtendedOffsetArm += CTimer::Dt() * 10.f;
			}
		}
	}
	else {
		mySettings.myMinDistance = 0.75f;
		myExtendedOffsetArm = 0.f;
	}

	//shoudl be changed to R instead of L
	if (Input::GetInstance()->IsKeyDown('R') && myHoldingAItem && myCurrentTarget.myRigidBodyPtr) {
		myIsRotatingmode = true;
		myCurrentTarget.myRigidBodyPtr->LockAngular(false);
		bool lockCamera = true;
		myObjectRotation = Vector2::Zero;
		CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
		myObjectRotation = Input::GetInstance()->GetAxisRaw();
		CEngine::GetInstance()->GetWindowHandler()->HidLockCursor(false);
		lockCamera = true;
	}
	else if (myHoldingAItem && myCurrentTarget.myRigidBodyPtr) {
		myIsRotatingmode = false;
		myObjectRotation = Vector2::Zero;
		myCurrentTarget.myRigidBodyPtr->LockAngular(true);
		bool lockCamera = false;
		CEngine::GetInstance()->GetWindowHandler()->HidLockCursor(true);
		CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
	}
	//else { // This ruins the camera shake :/
	//	myObjectRotation = Vector2::Zero;
	//	bool lockCamera = false;
	//	CMainSingleton::PostMaster().Send({ EMessageType::LockFPSCamera, &lockCamera });
	//}

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

void CGravityGloveComponent::RotateObject()
{
	myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	if (myJoint) {
		myJoint->release();
	}
	myJoint = nullptr;

	Matrix matrix;
	matrix.Forward(GameObject().myTransform->GetWorldMatrix().Forward());
	Matrix tempRotation = Matrix::CreateFromQuaternion(
		myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetRotation()
	);
	matrix = Matrix::CreateScale(myCurrentTarget.myRigidBodyPtr->GameObject().myTransform->Scale());
	matrix *= tempRotation;
	matrix.Translation(myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetPosition());


	float sensitivity = 0.075f;
	float x = std::clamp((myObjectRotation.y * sensitivity), ToDegrees(-PI / 2.0f) + 0.1f, ToDegrees(PI / 2.0f) - 0.1f);
	float y = WrapAngle((myObjectRotation.x * sensitivity));

	matrix = CTransformComponent::RotateMatrix(matrix, {x * 5.f, y * 5.f,0.f });

	DirectX::SimpleMath::Vector3 translation;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Quaternion quat;
	matrix.Decompose(scale, quat, translation);
	myCurrentTarget.myRigidBodyPtr->SetGlobalPose(myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetPosition(), quat);
	myCurrentTarget.myRigidBodyPtr->GetDynamicRigidBody()->GetBody().setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
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

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
{
	mySettings.myPushForce = 10.f;
	//mySettings.myDistanceToMaxLinearVelocity = 2.5f;
	mySettings.myMaxPushForce = 100.0f;
	mySettings.myMinPushForce = 10.0f;
	mySettings.myMinPullForce = 1.5f;

	mySettings.myMaxDistance = 2.0f;
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

	if (myCurrentTarget.myRigidBodyPtr != nullptr)
	{
		Vector3 direction = -(myCurrentTarget.myRigidBodyPtr->GameObject().myTransform->WorldPosition() - myGravitySlot->WorldPosition());
		float distance = direction.Length();
		float maxDistance = mySettings.myMaxDistance;

		mySettings.myCurrentDistanceInverseLerp = min(1.0f, InverseLerp(0.0f, maxDistance, distance));

		if (mySettings.myCurrentDistanceInverseLerp < 0.1f)
		{
			//dont remove pls - Alexander Matth�i 2021-04-30
			/*myJoint = PxD6JointCreate(*CEngine::GetInstance()->GetPhysx().GetPhysics(), myRigidStatic, myRigidStatic->getGlobalPose(), &myCurrentTarget->GetDynamicRigidBody()->GetBody(), myCurrentTarget->GetDynamicRigidBody()->GetBody().getGlobalPose());
			myJoint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);

			myJoint->setMotion(PxD6Axis::eY, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eZ, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			myJoint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);

			myJoint->setMotion(PxD6Axis::eX, PxD6Motion::eFREE);
			myJoint->setMotion(PxD6Axis::eY, PxD6Motion::eLIMITED);
			myJoint->setMotion(PxD6Axis::eZ, PxD6Motion::eLIMITED);*/

			//myCurrentTarget->SetPosition(myGravitySlot->WorldPosition());
			//myCurrentTarget->SetRotation(myCurrentTarget->GetComponent<CTransformComponent>()->Rotation());
			myCurrentTarget.myRigidBodyPtr->SetGlobalPose(myGravitySlot->WorldPosition(), myCurrentTarget.myRigidBodyPtr->GetComponent<CTransformComponent>()->Rotation());
			myCurrentTarget.myRigidBodyPtr->SetLinearVelocity({ 0.f, 0.f, 0.f });
			myCurrentTarget.myRigidBodyPtr->SetAngularVelocity({ 0.f, 0.f, 0.f });

			if (myCurrentTarget.myRigidBodyPtr->GetComponent<CHealthPickupComponent>()) {
				if (GameObject().myTransform->GetParent()->GetComponent<CPlayerComponent>()->CurrentHealth() < 100.f) {
					GameObject().myTransform->GetParent()->GetComponent<CPlayerComponent>()->IncreaseHealth(myCurrentTarget.myRigidBodyPtr->GetComponent<CHealthPickupComponent>()->GetHealthPickupAmount());
					myCurrentTarget.myRigidBodyPtr->GetComponent<CHealthPickupComponent>()->Destroy();
					myCurrentTarget.myRigidBodyPtr = nullptr;

					bool released = true;
					CMainSingleton::PostMaster().Send({ EMessageType::GravityGlovePull, &released });
				}
			}
		}
		else
		{
			float force = Lerp(mySettings.myMaxPushForce, mySettings.myMinPullForce, mySettings.myCurrentDistanceInverseLerp);

			//myCurrentTarget->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(max(mySettings.myDistanceToMaxLinearVelocity, distance));
			direction.Normalize();
			myCurrentTarget.myRigidBodyPtr->AddForce(direction, force, EForceMode::EForce);
		}
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
	if (aTransform == nullptr || aTransform->GetComponent<CEnemyComponent>())
	{
		return;
	}

	if (!aRigidBodyTarget->IsKinematic()) {
		myCurrentTarget.Clear();

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

	PxRaycastBuffer hit = CEngine::GetInstance()->GetPhysx().Raycast(start, dir, mySettings.myMaxDistance, CPhysXWrapper::ELayerMask::WORLD);// ELayerMask could be changed to ::DYNAMIC only? // Aki 2021 05 26
	if (hit.getNbAnyHits() > 0)
	{
		CTransformComponent* transform = static_cast<CTransformComponent*>(hit.getAnyHit(0).actor->userData);
		CRigidBodyComponent* rigidbody = nullptr;
		if (transform->GameObject().TryGetComponent<CRigidBodyComponent>(&rigidbody))
		{
			if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Right))
			{
				// Wind down
				//data.myIndex = 0;
				//data.myShouldBeReversed = true;
				Push(transform, rigidbody);
			}
			if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
			{
				Pull(transform, rigidbody);
			}
			else if (Input::GetInstance()->IsMouseReleased(Input::EMouseButton::Left))
			{
				Release();
			}

			if(myCurrentTarget.myRigidBodyPtr)
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

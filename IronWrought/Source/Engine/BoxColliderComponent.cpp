#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "PhysXWrapper.h"
#include "RigidBodyComponent.h"
#include "Engine.h"
#include "Scene.h"
#include "RigidDynamicBody.h"
#include "TransformComponent.h"
#include "PlayerControllerComponent.h"
#include "TeleporterComponent.h"

#include "LineFactory.h"
#include "LineInstance.h"

CBoxColliderComponent::CBoxColliderComponent(CGameObject& aParent, const Vector3& aPositionOffset, const Vector3& aBoxSize, const bool aIsTrigger, const unsigned int aLayerValue, PxMaterial* aMaterial)
	: CBehavior(aParent)
	, myShape(nullptr)
	, myPositionOffset(aPositionOffset)
	, myBoxSize(aBoxSize)
	, myMaterial(aMaterial)
	, myIsTrigger(aIsTrigger)
	, myLayerValue(aLayerValue)
	, myAudioEventIndex(-1)
	, myTriggerOnce(false)
	, myHasTriggered(false)
	, mySceneSection(-1)
	, myCanBeDeactivated(true)
	//, myEventFilter(static_cast<EEventFilter>(aLayerValue))
#ifdef DEBUG_COLLIDER_BOX
	, myColliderDraw(nullptr)
#endif
{
	if (myMaterial == nullptr) {
		myMaterial = CEngine::GetInstance()->GetPhysx().CreateMaterial(CPhysXWrapper::materialfriction::basic);
	}
}

CBoxColliderComponent::~CBoxColliderComponent()
{
#ifdef DEBUG_COLLIDER_BOX
	if (myColliderDraw)
		delete myColliderDraw;
#endif 
}

void CBoxColliderComponent::Awake()
{
	CreateBoxCollider();
}

void CBoxColliderComponent::Start()
{
}

//#define DEBUG_COLLIDER_BOX
void CBoxColliderComponent::Update()
{
#ifdef DEBUG_COLLIDER_BOX
	auto quatRot = myShape->getActor()->getGlobalPose().q;
	myColliderDraw->SetRotation(Quaternion(quatRot.x, quatRot.y, quatRot.z, quatRot.w));

	auto pos = myShape->getActor()->getGlobalPose().p;
	auto posShape = myShape->getLocalPose().p;
	Vector3 posPos = Vector3( pos.x /*+ myPositionOffset.x*/, pos.y + myPositionOffset.y, pos.z/* + (myBoxSize.x / 2.f)*/);
	//pos = posShape + pos;
	//Vector3 posPos = Vector3( pos.x  , pos.y , pos.z) + myPositionOffset;
	myColliderDraw->SetPosition(posPos);

	//myColliderDraw->SetPosition(GameObject().myTransform->Position() + myPositionOffset);
	//myColliderDraw->SetRotation(GameObject().myTransform->Rotation());
#endif
}

void CBoxColliderComponent::CreateBoxCollider()
{
	myShape = CEngine::GetInstance()->GetPhysx().GetPhysics()->createShape(physx::PxBoxGeometry(myBoxSize.x / 2.f, myBoxSize.y / 2.f, myBoxSize.z / 2.f), *myMaterial, true);
	Vector3 colliderPos =/* GameObject().myTransform->Position() +*/ myPositionOffset;


	myShape->setLocalPose({ -colliderPos.x, colliderPos.y, -colliderPos.z });
	PxFilterData filterData;
	CPhysXWrapper::ELayerMask layer = static_cast<CPhysXWrapper::ELayerMask>(myLayerValue);
	filterData.word0 = static_cast<PxU32>(layer);// static_cast<CPhysXWrapper::ELayerMask>(myLayerValue);// ::GROUP1;
	myShape->setQueryFilterData(filterData);
	//myShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

	CreateColliderDraw(myBoxSize.x / 2.f, myBoxSize.y / 2.f, myBoxSize.z / 2.f, myPositionOffset);

	CRigidBodyComponent* rigidBody = nullptr;
	if (GameObject().TryGetComponent(&rigidBody))
	{
		//rigidBody->SetPosition(GameObject().myTransform->Position());
		rigidBody->AttachShape(myShape);
		rigidBody->GetDynamicRigidBody()->GetBody();
	
		//dynamic.setMaxLinearVelocity(10.f);
	}
	else
	{
		DirectX::SimpleMath::Vector3 translation;
		DirectX::SimpleMath::Vector3 scale;
		DirectX::SimpleMath::Quaternion quat;
		DirectX::SimpleMath::Matrix transform = GameObject().GetComponent<CTransformComponent>()->GetLocalMatrix();
		transform.Decompose(scale, quat, translation);

		PxVec3 pos = { translation.x, translation.y , translation.z};

		PxQuat pxQuat = { quat.x, quat.y, quat.z, quat.w };
		
		PxRigidStatic* actor = CEngine::GetInstance()->GetPhysx().GetPhysics()->createRigidStatic({ pos, pxQuat });
		actor->attachShape(*myShape);
		CEngine::GetInstance()->GetPhysx().GetPXScene()->addActor(*actor);
		if (myIsTrigger) {
			PxShape* triggerShape;
			actor->getShapes(&triggerShape, 1);
			triggerShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			triggerShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
			triggerShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
			actor->userData = (void*) this;
		}
	}
}

void CBoxColliderComponent::OnTriggerEnter(CTransformComponent* aOther)
{
	if (Enabled() == false && myCanBeDeactivated)
		return;

	if (IRONWROUGHT->GetActiveScene().CurrentSection() == mySceneSection && PostMaster::CompareStringMessage(PostMaster::SMSG_SECTION, myEventMessage.c_str()))
	{
		return;
	}

	if (myEventFilter == EEventFilter::PlayerOnly)
	{
		if (aOther->GetComponent<CPlayerControllerComponent>() != nullptr)
		{
			myHasTriggered = true;

			if (myAudioEventIndex > -1)
			{
				CMainSingleton::PostMaster().SendLate({ EMessageType::PlayResearcherEvent, &myAudioEventIndex });
			}

			CTeleporterComponent* teleporter = nullptr;
			if (GameObject().TryGetComponent<CTeleporterComponent>(&teleporter))
			{
				teleporter->TriggerTeleport(true, aOther);
				return;
			}

			PostMaster::SBoxColliderEvenTriggerData data = {};
			data.myState = true;
			data.myCollidersTransform = GameObject().myTransform;
			data.myOthersTransform = aOther;
			data.mySceneSection = mySceneSection;
			SStringMessage message = { myEventMessage.c_str(), &data };
			CMainSingleton::PostMaster().Send(message);
		}
	}
	else
	{
		PostMaster::SBoxColliderEvenTriggerData data = {};
		data.myState = true;
		data.myCollidersTransform = GameObject().myTransform;
		data.myOthersTransform = aOther;
		data.mySceneSection = mySceneSection;
		SStringMessage message = { myEventMessage.c_str(), &data };
		CMainSingleton::PostMaster().Send(message);

		myHasTriggered = true;
	}

	if (myTriggerOnce)
		if (myHasTriggered)
			Enabled(false);
}

void CBoxColliderComponent::OnTriggerExit(CTransformComponent* aOther)
{
	if (Enabled() == false && myCanBeDeactivated)
		return;

	if (IRONWROUGHT->GetActiveScene().CurrentSection() == mySceneSection && PostMaster::CompareStringMessage(PostMaster::SMSG_SECTION, myEventMessage.c_str()))
	{
		return;
	}

	if (myEventFilter == EEventFilter::PlayerOnly)
	{
		if (aOther->GetComponent<CPlayerControllerComponent>() != nullptr)
		{
			myHasTriggered = true;

			if (myAudioEventIndex > -1)
			{
				CMainSingleton::PostMaster().SendLate({ EMessageType::PlayResearcherEvent, &myAudioEventIndex });
			}

			CTeleporterComponent* teleporter = nullptr;
			if (GameObject().TryGetComponent<CTeleporterComponent>(&teleporter))
			{
				teleporter->TriggerTeleport(false, aOther);
				return;
			}

			PostMaster::SBoxColliderEvenTriggerData data = {};
			data.myState = false;
			data.myCollidersTransform = GameObject().myTransform;
			data.myOthersTransform = aOther;
			data.mySceneSection = mySceneSection;
			SStringMessage message = { myEventMessage.c_str(), &data };
			CMainSingleton::PostMaster().Send(message);
		}
	}
	else
	{
		PostMaster::SBoxColliderEvenTriggerData data = {};
		data.myState = false;
		data.myCollidersTransform = GameObject().myTransform;
		data.myOthersTransform = aOther;
		data.mySceneSection = mySceneSection;
		SStringMessage message = { myEventMessage.c_str(), &data };
		CMainSingleton::PostMaster().Send(message);

		myHasTriggered = true;
	}

	if (myTriggerOnce)
		if (myHasTriggered)
			Enabled(false);
}

void CBoxColliderComponent::RegisterEventTriggerMessage(const std::string& aMessage)
{
	if (PostMaster::CompareStringMessage(PostMaster::SMSG_SECTION, aMessage.c_str()))
	{
		myCanBeDeactivated = false;
	}
	myEventMessage = aMessage;
}

void CBoxColliderComponent::RegisterEventTriggerFilter(const int& anEventFilter)
{
	if (anEventFilter > static_cast<int>(EEventFilter::Any) && anEventFilter < static_cast<int>(EEventFilter::PlayerOnly))
	{
		assert(false && "EventFilter is not within enum range!");
		myEventFilter = EEventFilter::None;
		return;
	}

	myEventFilter = static_cast<EEventFilter>(anEventFilter);
}

void CBoxColliderComponent::RegisterEventTriggerAudioIndex(const int& anAudioIndex)
{
	myAudioEventIndex = anAudioIndex;
}

void CBoxColliderComponent::RegisterEventTriggerOnce(const bool& aTriggerOnce)
{
	myTriggerOnce = aTriggerOnce;
}

void CBoxColliderComponent::RegisterSceneSection(const int aSceneSection)
{
	mySceneSection = aSceneSection;
}

void CBoxColliderComponent::CanBeDeactivated(const bool aCanBeDeactivated)
{
	myCanBeDeactivated = aCanBeDeactivated;
}

void CBoxColliderComponent::OnEnable()
{
}

void CBoxColliderComponent::OnDisable()
{
}

void CBoxColliderComponent::CreateColliderDraw(const float& aHalfSizeX, const float& aHalfSizeY, const float& aHalfSizeZ, const Vector3& aPosOffset)
{
#ifdef DEBUG_COLLIDER_BOX
	myColliderDraw = new CLineInstance();
	myColliderDraw->Init(CLineFactory::GetInstance()->CreateBox(aHalfSizeX, aHalfSizeY, aHalfSizeZ));
	myColliderDraw->SetPosition(GameObject().myTransform->Position() + aPosOffset);
	myColliderDraw->SetRotation(GameObject().myTransform->Rotation());
	IRONWROUGHT->GetActiveScene().AddInstance(myColliderDraw);
#else
	aHalfSizeX;
	aHalfSizeY;
	aHalfSizeZ;
	aPosOffset;
#endif
}

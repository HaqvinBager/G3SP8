#include "stdafx.h"
#include "RigidBodyComponent.h"
#include "PhysXWrapper.h"
#include "RigidDynamicBody.h"
#include "Engine.h"
#include "Scene.h"

//CRigidBodyComponent::CRigidBodyComponent(CGameObject& aParent)
//	: CComponent(aParent)
//	, myDynamicRigidBody(nullptr)
//{
//}

CRigidBodyComponent::CRigidBodyComponent(CGameObject& aParent, const float& aMass, const Vector3& aLocalCenterMass, const Vector3& aInertiaTensor, const bool aIsKinematic)
	: CComponent(aParent)
	, myDynamicRigidBody(nullptr)
	, myMass(aMass)
	, myLocalCenterMass(aLocalCenterMass)
	, myInertiaTensor(aInertiaTensor)
	, myIsKinematic(aIsKinematic)
	, myIsBeingHold(false)
{
}

CRigidBodyComponent::~CRigidBodyComponent()
{
	if (!myDynamicRigidBody) {
		myDynamicRigidBody = nullptr;
	}
}

//static int kinematicCount = 0;
void CRigidBodyComponent::Awake()
{
	myDynamicRigidBody = CEngine::GetInstance()->GetPhysx().CreateDynamicRigidbody(*GameObject().myTransform);
	myDynamicRigidBody->GetBody().setMass(myMass);
	myDynamicRigidBody->GetBody().setMassSpaceInertiaTensor({ myInertiaTensor.x, myInertiaTensor.y, myInertiaTensor.z });
	myDynamicRigidBody->GetBody().setCMassLocalPose({myLocalCenterMass.x, myLocalCenterMass.y, myLocalCenterMass.z});
	myDynamicRigidBody->GetBody().userData = (void*)GameObject().myTransform;

	// Debug test
	/*kinematicCount += static_cast<int>(myIsKinematic);
	std::cout << __FUNCTION__ << " b" << myIsKinematic << " " << kinematicCount << std::endl;*/

	myDynamicRigidBody->GetBody().setRigidBodyFlag(physx::PxRigidBodyFlag::Enum::eKINEMATIC, myIsKinematic);
	if (!myIsKinematic)
	{
		myDynamicRigidBody->GetBody().setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);
		myDynamicRigidBody->GetBody().putToSleep();
	}
}

void CRigidBodyComponent::Start()
{

}

void CRigidBodyComponent::Update()
{

	if (myDynamicRigidBody != nullptr) {
		if(myIsKinematic) {
			SetGlobalPose(GameObject().myTransform->Position(), GameObject().myTransform->Rotation());
		}
		else
		{
			GameObject().myTransform->Rotation(myDynamicRigidBody->GetRotation());
			GameObject().myTransform->Position(myDynamicRigidBody->GetPosition());
		}
	}

	if (myIsBeingHold) {
		myIsBeingHold = false;
	}

}

void CRigidBodyComponent::AddForce(const Vector3& aDirection, const EForceMode aForceMode)
{
	AddForce(PxVec3(aDirection.x, aDirection.y, aDirection.z), aForceMode);
}

void CRigidBodyComponent::AddForce(const Vector3& aNormalizedDirection, const float aForce, const EForceMode aForceMode)
{
	Vector3 aDirectionWithForce = aNormalizedDirection * aForce;
	AddForce(PxVec3(aDirectionWithForce.x, aDirectionWithForce.y, aDirectionWithForce.z), aForceMode);
}

void CRigidBodyComponent::AddAngularForce(const Vector3& aAngularForce, const EForceMode aForceMode)
{
	PxVec3 angularForce = PxVec3(aAngularForce.x, aAngularForce.y, aAngularForce.z);
	myDynamicRigidBody->GetBody().addTorque(angularForce, (PxForceMode::Enum)aForceMode);

}

void CRigidBodyComponent::SetAngularVelocity(const Vector3& aVelocity)
{
	myDynamicRigidBody->GetBody().setAngularVelocity({ aVelocity.x, aVelocity.y, aVelocity.z });
}

void CRigidBodyComponent::SetLinearVelocity(const Vector3& aVelocity)
{
	myDynamicRigidBody->GetBody().setLinearVelocity({ aVelocity.x, aVelocity.y, aVelocity.z });
}

void CRigidBodyComponent::LockAngular(bool aLock) {
	myDynamicRigidBody->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, aLock);
	myDynamicRigidBody->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, aLock);
	myDynamicRigidBody->GetBody().setRigidDynamicLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, aLock);
}

const float CRigidBodyComponent::GetLinearVelocityLengthSqr() const
{
	return myDynamicRigidBody->GetLinearVelocity().LengthSquared();
}

void CRigidBodyComponent::AddForce(const physx::PxVec3& aDirectionAndForce, const EForceMode aForceMode)
{
	if (myDynamicRigidBody->GetBody().isSleeping())
	{
		myDynamicRigidBody->GetBody().wakeUp();
	}
	myDynamicRigidBody->GetBody().addForce(aDirectionAndForce, (PxForceMode::Enum)aForceMode);
}

void CRigidBodyComponent::IsHeld(bool isHeld)
{
	myIsBeingHold = isHeld;
}

void CRigidBodyComponent::AttachShape(physx::PxShape* aShape)
{
	bool status = myDynamicRigidBody->GetBody().attachShape(*aShape);
	if (status) {
		//CEngine::GetInstance()->GetActiveScene().PXScene()->addActor(myDynamicRigidBody->GetBody());
	}
}

physx::PxShape* CRigidBodyComponent::GetShape()
{
	PxShape* shape = nullptr;
	myDynamicRigidBody->GetBody().getShapes(&shape, 1);
	return shape;
}

const float CRigidBodyComponent::GetMass() const
{
	return myDynamicRigidBody->GetBody().getMass();
}

void CRigidBodyComponent::Destroy()
{
	CEngine::GetInstance()->GetPhysx().GetPXScene()->removeActor(myDynamicRigidBody->GetBody());
	myDynamicRigidBody = nullptr;
}

void CRigidBodyComponent::SetPosition(const Vector3& aPos) {
	myDynamicRigidBody->GetBody().setGlobalPose({ { aPos.x, aPos.y, aPos.z },{ myDynamicRigidBody->GetRotation().x, myDynamicRigidBody->GetRotation().y, myDynamicRigidBody->GetRotation().z, myDynamicRigidBody->GetRotation().w}});
}

void CRigidBodyComponent::SetRotation(const Quaternion& aRot)
{
	PxQuat quat;
	quat.x = aRot.x;
	quat.y = aRot.y;
	quat.z = aRot.z;
	quat.w = aRot.w;
	myDynamicRigidBody->GetBody().setGlobalPose({ myDynamicRigidBody->GetPosition().x, myDynamicRigidBody->GetPosition().y, myDynamicRigidBody->GetPosition().z, quat });
}

void CRigidBodyComponent::SetGlobalPose(const Vector3& aPos, const Quaternion& aRot)
{
	PxVec3 pos;
	pos.x = aPos.x;
	pos.y = aPos.y;
	pos.z = aPos.z;
	PxQuat quat;
	quat.x = aRot.x;
	quat.y = aRot.y;
	quat.z = aRot.z;
	quat.w = aRot.w;
	myDynamicRigidBody->GetBody().setGlobalPose({pos, quat});
}

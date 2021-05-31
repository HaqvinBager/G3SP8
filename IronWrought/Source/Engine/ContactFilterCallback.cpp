#include "stdafx.h"
#include "ContactFilterCallback.h"
#include "TransformComponent.h"
#include "PhysicsPropAudioComponent.h"
#include "RigidBodyComponent.h"
#include "RigidDynamicBody.h"

physx::PxFilterFlags CContactFilterCallback::pairFound(physx::PxU32 /*pairID*/, physx::PxFilterObjectAttributes /*attributes0*/, physx::PxFilterData /*filterData0*/, const physx::PxActor* a0, const physx::PxShape* /*s0*/, physx::PxFilterObjectAttributes /*attributes1*/, physx::PxFilterData /*filterData1*/, const physx::PxActor* a1, const physx::PxShape* /*s1*/, physx::PxPairFlags& /*pairFlags*/)
{
	CTransformComponent* firstTransform = (CTransformComponent*)a0->userData;
	CTransformComponent* secondTransform = (CTransformComponent*)a1->userData;
	CPhysicsPropAudioComponent* audioComponent = nullptr;
	if (firstTransform) {
		audioComponent = firstTransform->GameObject().GetComponent<CPhysicsPropAudioComponent>();
	}
	else {
		audioComponent = secondTransform->GameObject().GetComponent<CPhysicsPropAudioComponent>();
	}
	if (!audioComponent)
		audioComponent = secondTransform->GameObject().GetComponent<CPhysicsPropAudioComponent>();

	if (audioComponent)
	{
		float velocity = audioComponent->GetComponent<CRigidBodyComponent>()->GetDynamicRigidBody()->GetLinearVelocity().LengthSquared();
		if (velocity > 0.05f)
		{
			if (audioComponent->Ready()) {
				CMainSingleton::PostMaster().SendLate({ EMessageType::PhysicsPropCollision, &audioComponent->GetSoundIndex() });
			}
		}
	}
    return physx::PxFilterFlags();
}

void CContactFilterCallback::pairLost(physx::PxU32 /*pairID*/, physx::PxFilterObjectAttributes /*attributes0*/, physx::PxFilterData /*filterData0*/, physx::PxFilterObjectAttributes /*attributes1*/, physx::PxFilterData /*filterData1*/, bool /*objectRemoved*/)
{
}

bool CContactFilterCallback::statusChange(physx::PxU32& /*pairID*/, physx::PxPairFlags& /*pairFlags*/, physx::PxFilterFlags& /*filterFlags*/)
{
    return false;
}
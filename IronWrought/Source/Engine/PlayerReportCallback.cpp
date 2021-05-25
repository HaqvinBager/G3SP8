#include "stdafx.h"
#include "PlayerReportCallback.h"
#include "TransformComponent.h"
#include "RigidBodyComponent.h"
#include <PlayerControllerComponent.h>
#include <HealthPickupComponent.h>
#include <FuseComponent.h>
#include <FuseboxComponent.h>
#include <PlayerComponent.h>
#include "RigidDynamicBody.h"
#include <EnemyComponent.h>

void CPlayerReportCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
{
	//CTransformComponent* transform = (CTransformComponent*)hit.actor->userData;
	/*CRigidBodyComponent* rigid = transform->GameObject().GetComponent<CRigidBodyComponent>();
	if (rigid) {
		std::cout << rigid->GameObject().InstanceID() << std::endl;
	}*/
	//anv�nder normalen ist�llet f�r velocity f�r det puttas b�ttre �t de h�ll man g�r in i
	CTransformComponent* playerTransform = (CTransformComponent*)hit.controller->getUserData();
	if (hit.shape->getGeometryType() == physx::PxGeometryType::eTRIANGLEMESH) {
		myHitNormal = hit.worldNormal;
	}
	if (playerTransform) {
		if (playerTransform->GameObject().GetComponent<CPlayerControllerComponent>()) {
			if (hit.actor->userData != nullptr)
				/*if (hit.shape->getGeometryType() != physx::PxGeometryType::eTRIANGLEMESH)*/ {
				CTransformComponent* objectTransform = (CTransformComponent*)hit.actor->userData;
				CPlayerControllerComponent* player = playerTransform->GameObject().GetComponent<CPlayerControllerComponent>();
				if (player) {
					if (objectTransform) {

						if (objectTransform->GetComponent<CHealthPickupComponent>()) 
						{
							if (player->GetComponent<CPlayerComponent>()->CurrentHealth() < 100.f) 
							{
								player->GetComponent<CPlayerComponent>()->IncreaseHealth(objectTransform->GetComponent<CHealthPickupComponent>()->GetHealthPickupAmount());
								objectTransform->GetComponent<CHealthPickupComponent>()->Destroy();
							
								CMainSingleton::PostMaster().SendLate({ EMessageType::PlayerHealthPickup, nullptr }); // For AudioManager
							}
						}
						else if (objectTransform->GetComponent<CFuseComponent>()) 
						{
							objectTransform->GetComponent<CFuseComponent>()->OnPickUp();
						}
						else if (objectTransform->GetComponent<CFuseboxComponent>()) 
						{
							objectTransform->GetComponent<CFuseboxComponent>()->RunEvent();
						}

						Vector3 v = player->GetLinearVelocity();
						Vector3 n = { hit.worldNormal.x, hit.worldNormal.y, hit.worldNormal.z };
						float dot = n.Dot({ 0,1,0 });
						float angle = std::acos(dot) * (180.f / 3.14f);
						CRigidBodyComponent* other = objectTransform->GetComponent<CRigidBodyComponent>();
						//std::cout << angle << std::endl;
						if (other != nullptr && angle > 50.f && other->GetDynamicRigidBody() != nullptr) 
						{
							if (!other->IsKinematic())
							{
								float m = other->GetMass();
								Vector3 f = { (m * ((v - n) / CTimer::Dt())) };
								other->AddForce(f / 50.f);
								//other->GetDynamicRigidBody()->GetBody().setMaxLinearVelocity(10.f);
								//F = m * (v - v0/t - t0) or F = m * (v/t) because v0 and t0 is almost always 0 in this case
								//m = mass
								//v = velocity
								//t = time
							}
							//else
							//{
							//	playerTransform->GetComponent<CPlayerControllerComponent>()->LadderEnter(nullptr);
							//}
						}

						if (n.y > 0.99f) // Floor Check
						{
							player->SetCurrentFloorMaterial(objectTransform->GameObject().Tag());
						}
					}
				}
			}
		}
	}
}

void CPlayerReportCallback::onControllerHit(const physx::PxControllersHit& /*hit*/)
{
}

void CPlayerReportCallback::onObstacleHit(const physx::PxControllerObstacleHit& /*hit*/)
{
	/*CTransformComponent* transform = (CTransformComponent*)hit.controller->getActor()->userData;
	CRigidBodyComponent* rigid = transform->GameObject().GetComponent<CRigidBodyComponent>();
	if (rigid) {
		std::cout << rigid->GameObject().InstanceID() << std::endl;
	}*/
}

const Vector3 CPlayerReportCallback::GetNormal() const
{
	return Vector3(myHitNormal.x, myHitNormal.y, myHitNormal.z);
}

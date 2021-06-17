#include "stdafx.h"
#include "InteractionBehavior.h"
#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "PhysXWrapper.h"
#include "KeyBehavior.h"
#include "KeyBehavior.h"
#include "LeftClickDownLock.h"
#include "BoxColliderComponent.h"
#include "OnLookLock.h"

CInteractionBehavior::CInteractionBehavior(CGameObject& aParent)
	: CBehavior(aParent)
	, myUpdateEyes(true)
{
}

CInteractionBehavior::~CInteractionBehavior()
{
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Unsubscribe(PostMaster::SMSG_ENABLE_GLOVE, this);
}

void CInteractionBehavior::OnEnable()
{
}

void CInteractionBehavior::Awake()
{
}

void CInteractionBehavior::Start()
{
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_DISABLE_GLOVE, this);
	CMainSingleton::PostMaster().Subscribe(PostMaster::SMSG_ENABLE_GLOVE, this);
}

void CInteractionBehavior::Update()
{
	UpdateEyes();
}

void CInteractionBehavior::OnDisable()
{
}

void CInteractionBehavior::Receive(const SStringMessage& aMsg)
{
	if (PostMaster::CompareStringMessage(PostMaster::SMSG_DISABLE_GLOVE, aMsg.myMessageType))
	{
		myUpdateEyes = false;
		return;
	}
	if (PostMaster::CompareStringMessage(PostMaster::SMSG_ENABLE_GLOVE, aMsg.myMessageType))
	{
		myUpdateEyes = true;
		return;
	}
}

void CInteractionBehavior::UpdateEyes()
{
	if (!myUpdateEyes)
	{
		return;
	}

	Vector3 origin = GameObject().myTransform->WorldPosition();
	Vector3 direction = -GameObject().myTransform->GetWorldMatrix().Forward();
	auto hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 2.75f, CPhysXWrapper::ELayerMask::DYNAMIC_OBJECTS);
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		if (hit.hasAnyHits())
		{
			const auto& result = hit.getAnyHit(0);
			CTransformComponent* hitTransform = static_cast<CTransformComponent*>(result.actor->userData);
			if (hitTransform != nullptr)
			{
				CKeyBehavior* key = nullptr;
				if (hitTransform->GameObject().TryGetComponentAny(&key))
				{
					CBoxColliderComponent* boxCollider = nullptr;
					if (key->GameObject().TryGetComponentAny(&boxCollider))
					{
						if (boxCollider->Enabled()) {
							CMainSingleton::PostMaster().SendLate({ EMessageType::FoundKey, &key->GameObject() });
							key->TriggerActivations();
						}
					}
				}

				CLeftClickDownLock* leftClickDownLock = nullptr;
				if (hitTransform->GameObject().TryGetComponentAny(&leftClickDownLock))
				{
					CBoxColliderComponent* boxCollider = nullptr;
					if (leftClickDownLock->GameObject().TryGetComponentAny(&boxCollider))
					{
						if (boxCollider->Enabled()) {
							CMainSingleton::PostMaster().SendLate({ EMessageType::FoundKey, &leftClickDownLock->GameObject() });
							leftClickDownLock->ActivateEvent();
						}
					}
				}
			}
		}
	}
	else
	{
		if (hit.hasAnyHits())
		{
			const auto& result = hit.getAnyHit(0);
			CTransformComponent* hitTransform = static_cast<CTransformComponent*>(result.actor->userData);
			if (hitTransform != nullptr)
			{
				COnLookLock* onLookLock = nullptr;
				if (hitTransform->GameObject().TryGetComponentAny(&onLookLock))
					onLookLock->ActivateEvent();
				}
			}
		}
	}
}
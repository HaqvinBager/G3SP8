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
{
}

CInteractionBehavior::~CInteractionBehavior()
{
}

void CInteractionBehavior::OnEnable()
{
}

void CInteractionBehavior::Awake()
{
}

void CInteractionBehavior::Start()
{
}

void CInteractionBehavior::Update()
{
	UpdateEyes();
}

void CInteractionBehavior::OnDisable()
{
}

void CInteractionBehavior::UpdateEyes()
{
	Vector3 origin = GameObject().myTransform->WorldPosition();
	Vector3 direction = -GameObject().myTransform->GetWorldMatrix().Forward();
	auto hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 2.0f, CPhysXWrapper::ELayerMask::DYNAMIC_OBJECTS);
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		if (hit.hasAnyHits())
		{
			const auto& result = hit.getAnyHit(0);
			CTransformComponent* hitTransform = static_cast<CTransformComponent*>(result.actor->userData);
			if (hitTransform != nullptr)
			{
				CKeyBehavior* key = nullptr;
				if (hitTransform->GameObject().TryGetComponent(&key))
				{
					CBoxColliderComponent* boxCollider = nullptr;
					if (key->GameObject().TryGetComponent(&boxCollider))
					{
						if (boxCollider->Enabled())
							key->TriggerActivations();
					}
				}

				CLeftClickDownLock* leftClickDownLock = nullptr;
				if (hitTransform->GameObject().TryGetComponent(&leftClickDownLock))
				{
					CBoxColliderComponent* boxCollider = nullptr;
					if (leftClickDownLock->GameObject().TryGetComponent(&boxCollider))
					{
						if(boxCollider->Enabled())
							leftClickDownLock->ActivateEvent();
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
				if (hitTransform->GameObject().TryGetComponent(&onLookLock))
				{
					onLookLock->ActivateEvent();
				}
			}
		}
	}
}
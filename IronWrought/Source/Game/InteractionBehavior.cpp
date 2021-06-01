#include "stdafx.h"
#include "InteractionBehavior.h"
#include "Component.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "Engine.h"
#include "PhysXWrapper.h"
#include "DestroyKey.h"

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
	if (Input::GetInstance()->IsMousePressed(Input::EMouseButton::Left))
	{
		Vector3 origin = GameObject().myTransform->WorldPosition();
		Vector3 direction = -GameObject().myTransform->GetWorldMatrix().Forward();
		auto hit = CEngine::GetInstance()->GetPhysx().Raycast(origin, direction, 2.0f, CPhysXWrapper::ELayerMask::DYNAMIC_OBJECTS);
		if (hit.hasAnyHits())
		{
			const auto& result = hit.getAnyHit(0);
			CTransformComponent* hitTransform = static_cast<CTransformComponent*>(result.actor->userData);
			if (hitTransform != nullptr)
			{
				CDestroyKey* key = nullptr;
				if (hitTransform->GameObject().TryGetComponent(&key))
				{
					key->OnInteract();
				}
			}
		}
	}
}
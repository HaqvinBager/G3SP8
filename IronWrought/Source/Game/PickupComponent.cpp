#include "stdafx.h"
#include "PickupComponent.h"
#include "RigidBodyComponent.h"
#include "Scene.h"

CPickupComponent::CPickupComponent(CGameObject& aParent) : CBehaviour(aParent)
{
}

CPickupComponent::~CPickupComponent()
{
}

void CPickupComponent::Destroy()
{
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

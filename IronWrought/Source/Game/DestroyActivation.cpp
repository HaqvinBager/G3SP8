#include "stdafx.h"
#include "DestroyActivation.h"
#include "RigidBodyComponent.h"
#include "Scene.h"

CDestroyActivation::CDestroyActivation(CGameObject& aParent) : IActivationBehavior(aParent)
{

}

CDestroyActivation::~CDestroyActivation()
{

}

void CDestroyActivation::OnActivation()
{
	GameObject().GetComponent<CRigidBodyComponent>()->Destroy();
	CEngine::GetInstance()->GetActiveScene().RemoveInstance(&GameObject());
}

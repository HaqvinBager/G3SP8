#include "stdafx.h"
#include "AddForceResponse.h"
#include <RigidBodyComponent.h>

CAddForceResponse::CAddForceResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
{
	HasBeenActivated(false);
}

CAddForceResponse::~CAddForceResponse()
{
}

void CAddForceResponse::FixedUpdate()
{
	if (HasBeenActivated())
	{
		CRigidBodyComponent* rigidBody = nullptr;
		if (GameObject().TryGetComponent(&rigidBody))
		{
			rigidBody->AddForce(mySettings.myDirection, mySettings.myForce, EForceMode::EImpulse);
		}
		Enabled(false);
	}
}

void CAddForceResponse::OnRespond()
{
	if (Enabled())
	{
		HasBeenActivated(true);
	}
}

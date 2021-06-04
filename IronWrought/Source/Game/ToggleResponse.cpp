#include "stdafx.h"
#include "Scene.h"
#include "ToggleResponse.h"
#include <PointLightComponent.h>
#include <SpotLightComponent.h>
#include <BoxColliderComponent.h>
#include <ModelComponent.h>
#include "RotateActivation.h"
#include "MoveActivation.h"
#include "DestroyActivation.h"
#include "AudioActivation.h"
#include "InstantActivationLock.h"
#include "LeftClickDownLock.h"
#include "OnLookLock.h"
#include "OnTriggerLock.h"

CToggleResponse::CToggleResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, myTarget(nullptr)
	, mySettings(someSettings)
{
}

CToggleResponse::~CToggleResponse()
{
}

void CToggleResponse::Awake()
{
	CGameObject* target = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(mySettings.myTargetInstanceID);
	if (target)
	{
		const std::string& type = mySettings.myType;
		if (type.find("BoxCollider") != std::string::npos)
		{
			SetTarget<CBoxColliderComponent>(*target);
		}
		else if (type.find("Point") != std::string::npos)
		{
			SetTarget<CPointLightComponent>(*target);
		}
		else if (type.find("Spot") != std::string::npos)
		{
			SetTarget<CSpotLightComponent>(*target);
		}
		else if (type.find("MeshFilter") != std::string::npos)
		{
			SetTarget<CModelComponent>(*target);
		}
		else if (type.find("ActivationRotate") != std::string::npos)
		{
			SetTarget<CRotateActivation>(*target);
		}
		else if (type.find("ActivationMove") != std::string::npos)
		{
			SetTarget<CMoveActivation>(*target);
		}
	}
}

void CToggleResponse::Start()
{
	if (myTarget != nullptr)
		myTarget->Enabled(mySettings.myEnableOnStart);
}

void CToggleResponse::Update()
{

}

void CToggleResponse::OnRespond()
{
	if (myTarget != nullptr)
	{
		myTarget->Enabled(mySettings.myEnableOnNotify);
	}

	/*
		CMainSingleton::PostMaster().SendLate({ EMessageType::WhateverMessageThatMakesSense, myAudioIndex });
	*/
}
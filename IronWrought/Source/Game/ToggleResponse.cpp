#include "stdafx.h"
#include "ToggleResponse.h"
#include <PointLightComponent.h>
#include <BoxColliderComponent.h>

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
	const std::string& type = mySettings.myType;
	if (type.find("Light") != std::string::npos)
	{
		SetTarget<CPointLightComponent>();
	}
	else if (type.find("BoxColllider") != std::string::npos)
	{
		SetTarget<CBoxColliderComponent>();
	}


	/* 
	//Example of how adding additional types could look like
	else if(type.find("BoxCollider") != std::string::npos)
	{
		SetTarget<CBoxColliderComponent>();
	}
	*/
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
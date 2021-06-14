#include "stdafx.h"
#include "LightActivation.h"
#include <PointLightComponent.h>
#include <SpotLightComponent.h>

#include <PointLight.h>
#include <SpotLight.h>

#include "Scene.h"

CLightActivation::CLightActivation(CGameObject& aParent, const SData& aData)
	: IActivationBehavior(aParent)
	, myData(aData)
{
}

CLightActivation::~CLightActivation()
{
}

void CLightActivation::Awake()
{
	CGameObject* targetGameObject = CEngine::GetInstance()->GetActiveScene().FindObjectWithID(myData.myTargetInstanceID);
	if (myData.myTargetType == "Point")
	{
		SetTarget<CPointLightComponent>(*targetGameObject);
	}
	else if (myData.myTargetType == "Spot")
	{
		SetTarget<CSpotLightComponent>(*targetGameObject);
	}
}

void CLightActivation::OnActivation()
{
	if (myData.myTargetType == "Point")
	{
		CPointLight* pointLight = static_cast<CPointLightComponent*>(myTarget)->GetPointLight();
		pointLight->SetColor(myData.myColor);
		pointLight->SetIntensity(myData.myIntensity);
	}
	else if (myData.myTargetType == "Spot")
	{
		CSpotLight* spotLight = static_cast<CSpotLightComponent*>(myTarget)->GetSpotLight();
		spotLight->SetColor(myData.myColor);
		spotLight->SetIntensity(myData.myIntensity);
	}
}

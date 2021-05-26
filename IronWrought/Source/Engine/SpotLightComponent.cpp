#include "stdafx.h"
#include "SpotLightComponent.h"
#include "GameObject.h"
#include "SpotLight.h"
#include "LightFactory.h"
#include "TransformComponent.h"
#include <BinReader.h>

CSpotLightComponent::CSpotLightComponent(CGameObject& aParent, const Binary::SSpotLight& aData)
	: CBehaviour(aParent)
	, myColor(aData.color)
	, myIntensity(aData.intensity)
	, myDirection(GameObject().myTransform->Transform().Forward())
	, myRange(aData.range)
	, myWideness(aData.outerSpotAngle)
	//, myInnerWidness(aData.innerSpotAngle) ? =)) /Axel Savage 26/05/2021
{
	mySpotLight = CLightFactory::GetInstance()->CreateSpotLight();
	mySpotLight->SetPosition(GameObject().myTransform->Position());
	mySpotLight->SetColor(myColor);
	mySpotLight->SetIntensity(myIntensity);
	mySpotLight->SetRange(myRange);
	mySpotLight->SetDirection(myDirection);
	mySpotLight->SetWideness(myWideness);
}

CSpotLightComponent::CSpotLightComponent(CGameObject& aParent, Vector3 aColor, float anIntensity, Vector3 aDirection, float aRange, float aWideness)
	: CBehaviour(aParent)
	, myColor(aColor)
	, myIntensity(anIntensity)
	, myDirection(aDirection)
	, myRange(aRange)
	, myWideness(aWideness)
{
	mySpotLight = CLightFactory::GetInstance()->CreateSpotLight();
	mySpotLight->SetPosition(GameObject().myTransform->Position());
	mySpotLight->SetColor(myColor);
	mySpotLight->SetIntensity(myIntensity);
	mySpotLight->SetRange(myRange);
	mySpotLight->SetDirection(myDirection);
	mySpotLight->SetWideness(myWideness);
}

CSpotLightComponent::~CSpotLightComponent()
{
}

void CSpotLightComponent::Awake()
{
}

void CSpotLightComponent::Start()
{
}

void CSpotLightComponent::Update()
{
	//mySpotLight->SetPosition(GameObject().myTransform->Position());
	//mySpotLight->SetDirection(GameObject().myTransform->Transform().Forward());
}

void CSpotLightComponent::OnEnable()
{
}

void CSpotLightComponent::OnDisable()
{
}

CSpotLight* CSpotLightComponent::GetSpotLight() const
{
	return mySpotLight;
}

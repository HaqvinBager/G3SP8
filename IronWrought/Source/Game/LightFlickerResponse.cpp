#include "stdafx.h"
#include "LightFlickerResponse.h"
#include <SpotLightComponent.h>
#include <PointLightComponent.h>
#include <Behavior.h>
#include <PointLight.h>
#include <SpotLight.h>

CLightFlickerResponse::CLightFlickerResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, mySettings(someSettings)
{
	myNoise = FastNoise::New<FastNoise::Simplex>();
	mySeed = abs(rand());
}

CLightFlickerResponse::~CLightFlickerResponse()
{
}

void CLightFlickerResponse::Awake()
{
	HasBeenActivated(false);

	CPointLightComponent* point = nullptr;
	CSpotLightComponent* spot = nullptr;
	if (GameObject().TryGetComponent(&point))
	{
		myLightType = 0;
		myLight = static_cast<CBehavior*>(point);
	}
	else if (GameObject().TryGetComponent(&spot))
	{
		myLightType = 1;
		myLight = static_cast<CBehavior*>(spot);
	}

	std::cout << __FUNCTION__ << " Seed = " << mySeed << std::endl;
}

void CLightFlickerResponse::Update()
{
	if (HasBeenActivated())
	{
		UpdateNoise();

		switch (myLightType)
		{
		case 0:
			{
				CPointLightComponent* light = static_cast<CPointLightComponent*>(myLight);
				light->GetPointLight()->SetIntensity(Noise());
			}
			break;

		case 1:
			{
				CSpotLightComponent* light = static_cast<CSpotLightComponent*>(myLight);
				light->GetSpotLight()->SetIntensity(Noise());
			}
			break;
		}
	}
}

void CLightFlickerResponse::OnRespond()
{
	HasBeenActivated(true);
}

float CLightFlickerResponse::Noise() const
{
	return myIntensity;
}

void CLightFlickerResponse::UpdateNoise() 
{
	float time = CTimer::Time() * mySettings.mySpeed;
	//int32_t seed = 1462;
	float noiseValue = InverseLerp(-1.0f, 1.0f, myNoise->GenSingle2D(time, time, mySeed));
	myIntensity = Remap(-1.0f, 1.0f, mySettings.myIntensity.x, mySettings.myIntensity.y, noiseValue);	
}
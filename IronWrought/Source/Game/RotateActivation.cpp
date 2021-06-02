#include "stdafx.h"
#include "RotateActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"

#define PI 3.141592f

CRotateActivation::CRotateActivation(CGameObject& aParent, const SSettings& someSettings) 
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myIsInteracted(false)
	, myTime(0.0f)
{
	mySettings.myStartRotation.x = (-mySettings.myStartRotation.x) - 360.0f;
	mySettings.myStartRotation.y += 180.0f;
	mySettings.myStartRotation.z = (-mySettings.myStartRotation.z) - 360.0f;
	mySettings.myStartRotation *= (PI / 180.0f);

	mySettings.myEndRotation.x = (-mySettings.myEndRotation.x) - 360.0f;
	mySettings.myEndRotation.y += 180.0f;
	mySettings.myEndRotation.z = (-mySettings.myEndRotation.z) - 360.0f;
	mySettings.myEndRotation *= (PI / 180.0f);
}

CRotateActivation::~CRotateActivation()
{
}

void CRotateActivation::OnActivation()
{
	myIsInteracted = true;
}

void CRotateActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();

		Quaternion rotation;
		static Quaternion startRotation = Quaternion::CreateFromYawPitchRoll(mySettings.myStartRotation.y, mySettings.myStartRotation.x, mySettings.myStartRotation.z);
		static Quaternion endRotation = Quaternion::CreateFromYawPitchRoll(mySettings.myEndRotation.y, mySettings.myEndRotation.x, mySettings.myEndRotation.z);

		if (mySettings.myDuration >= 0.0001f)
		{
			rotation = Quaternion::Slerp(startRotation, endRotation, myTime / mySettings.myDuration);
		}
		else
		{
			rotation = endRotation;
		}

		GameObject().myTransform->Rotation(rotation);

		if (myTime >= mySettings.myDuration)
			Enabled(false);
	}
}

#include "stdafx.h"
#include "AnimateActivation.h"
#include "TransformComponent.h"
#include "GameObject.h"

#define PI 3.141592f

CAnimateActivation::CAnimateActivation(CGameObject& aParent, const SSettings& someSettings) 
	: IActivationBehavior(aParent)
	, mySettings(someSettings)
	, myIsInteracted(false)
	, myTime(0.0f)
{
	mySettings.myStartPosition = GameObject().myTransform->Position() + mySettings.myStartPosition;
	mySettings.myEndPosition = GameObject().myTransform->Position() + mySettings.myEndPosition;

	mySettings.myStartRotation.x = (-mySettings.myStartRotation.x) - 360.0f;
	mySettings.myStartRotation.y += 180.0f;
	mySettings.myStartRotation.z = (-mySettings.myStartRotation.z) - 360.0f;
	mySettings.myStartRotation *= (PI / 180.0f);

	mySettings.myEndRotation.x = (-mySettings.myEndRotation.x) - 360.0f;
	mySettings.myEndRotation.y += 180.0f;
	mySettings.myEndRotation.z = (-mySettings.myEndRotation.z) - 360.0f;
	mySettings.myEndRotation *= (PI / 180.0f);
}

CAnimateActivation::~CAnimateActivation()
{
}



void CAnimateActivation::OnActivation()
{
	/*CMainSingleton::PostMaster().Send({ mySettings.myInteractNotify.c_str(), mySettings.myData });*/
	myIsInteracted = true;
}

void CAnimateActivation::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();

		Quaternion rotation;
		static Quaternion startRotation = Quaternion::CreateFromYawPitchRoll(mySettings.myStartRotation.y, mySettings.myStartRotation.x, mySettings.myStartRotation.z);
		//startRotation *= GameObject().myTransform->Rotation();
		static Quaternion endRotation = Quaternion::CreateFromYawPitchRoll(mySettings.myEndRotation.y, mySettings.myEndRotation.x, mySettings.myEndRotation.z);
		//endRotation *= GameObject().myTransform->Rotation();

		Vector3 position;

		if (mySettings.myDuration >= 0.0001f)
		{
			position = Vector3::Lerp(mySettings.myStartPosition, mySettings.myEndPosition, myTime / mySettings.myDuration);
			rotation = Quaternion::Slerp(startRotation, endRotation, myTime / mySettings.myDuration);
		}
		else
		{
			position = mySettings.myEndPosition;
			rotation = endRotation;
		}

		GameObject().myTransform->Position(position);
		GameObject().myTransform->Rotation(rotation);

		if (myTime >= mySettings.myDuration)
			Enabled(false);
	}
}

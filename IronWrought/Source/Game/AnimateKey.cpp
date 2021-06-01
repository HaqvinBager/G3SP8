#include "stdafx.h"
#include "AnimateKey.h"
#include "TransformComponent.h"
#define PI 3.141592f

CAnimateKey::CAnimateKey(CGameObject& aParent, const SSettings& someSettings, const SAnimateKeySettings& someAnimateKeySettings) 
	: CKeyBehavior(aParent, someSettings)
	, myAnimateKeySettings(someAnimateKeySettings)
	, myIsInteracted(false)
	, myTime(0.0f)
{
	myAnimateKeySettings.myStartRotation.x = (-myAnimateKeySettings.myStartRotation.x) - 360.0f;
	myAnimateKeySettings.myStartRotation.y += 180.0f;
	myAnimateKeySettings.myStartRotation.z = (-myAnimateKeySettings.myStartRotation.z) - 360.0f;
	myAnimateKeySettings.myStartRotation *= (PI / 180.0f);

	myAnimateKeySettings.myEndRotation.x = (-myAnimateKeySettings.myEndRotation.x) - 360.0f;
	myAnimateKeySettings.myEndRotation.y += 180.0f;
	myAnimateKeySettings.myEndRotation.z = (-myAnimateKeySettings.myEndRotation.z) - 360.0f;
	myAnimateKeySettings.myEndRotation *= (PI / 180.0f);
}

CAnimateKey::~CAnimateKey()
{
}



void CAnimateKey::OnInteract()
{
	CMainSingleton::PostMaster().Send({ mySettings.myInteractNotify.c_str(), mySettings.myData });
	myIsInteracted = true;
}

void CAnimateKey::Update()
{
	if (myIsInteracted)
	{
		myTime += CTimer::Dt();

		Quaternion rotation;
		Quaternion startRotation = Quaternion::CreateFromYawPitchRoll(myAnimateKeySettings.myStartRotation.y, myAnimateKeySettings.myStartRotation.x, myAnimateKeySettings.myStartRotation.z);
		Quaternion endRotation = Quaternion::CreateFromYawPitchRoll(myAnimateKeySettings.myEndRotation.y, myAnimateKeySettings.myEndRotation.x, myAnimateKeySettings.myEndRotation.z);

		Vector3 position;

		if (myAnimateKeySettings.myDuration >= 0.0001f)
		{
			position = Vector3::Lerp(myAnimateKeySettings.myStartPosition, myAnimateKeySettings.myEndPosition, myTime / myAnimateKeySettings.myDuration);
			rotation = Quaternion::Slerp(startRotation, endRotation, myTime / myAnimateKeySettings.myDuration);
		}
		else
		{
			position = myAnimateKeySettings.myEndPosition;
			rotation = endRotation;
		}

		GameObject().myTransform->Position(position);
		GameObject().myTransform->Rotation(rotation);

		if (myTime >= myAnimateKeySettings.myDuration)
			Enabled(false);
	}
}

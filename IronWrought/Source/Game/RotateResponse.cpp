#include "stdafx.h"
#include "RotateResponse.h"
#include "TransformComponent.h"

CRotateResponse::CRotateResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
	mySettings.myStartRotation.x = (-mySettings.myStartRotation.x) - 360.0f;
	mySettings.myStartRotation.y += 180.0f;
	mySettings.myStartRotation.z = (-mySettings.myStartRotation.z) - 360.0f;

	mySettings.myEndRotation.x = (-mySettings.myEndRotation.x) - 360.0f;
	mySettings.myEndRotation.y += 180.0f;
	mySettings.myEndRotation.z = (-mySettings.myEndRotation.z) - 360.0f;

	myStartRotation = GameObject().myTransform->Rotation();

	Enabled(false);
}

void CRotateResponse::Update()
{
	myTime += CTimer::Dt();

	Quaternion rotation;
	Quaternion startRotation = Quaternion::CreateFromYawPitchRoll(mySettings.myStartRotation.y, mySettings.myStartRotation.x, mySettings.myStartRotation.z);
	Quaternion endRotation = Quaternion::CreateFromYawPitchRoll(mySettings.myEndRotation.y, mySettings.myEndRotation.x, mySettings.myEndRotation.z);
	rotation = Quaternion::Slerp(startRotation, endRotation, myTime / mySettings.myDuration);

	GameObject().myTransform->Rotation(rotation);

	if (myTime >= mySettings.myDuration)
		Enabled(false);
}

void CRotateResponse::OnRespond()
{
	Enabled(true);
}
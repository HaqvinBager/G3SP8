#include "stdafx.h"
#include "RotateResponse.h"
#include "TransformComponent.h"
#define PI 3.141592f

CRotateResponse::CRotateResponse(CGameObject& aParent, const SSettings& someSettings)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
	mySettings.myStartRotation.x = (-mySettings.myStartRotation.x) - 360.0f;
	mySettings.myStartRotation.y += 180.0f;
	mySettings.myStartRotation.z = (-mySettings.myStartRotation.z) - 360.0f;
	mySettings.myStartRotation *= (PI / 180.0f);

	mySettings.myEndRotation.x = (-mySettings.myEndRotation.x) - 360.0f;
	mySettings.myEndRotation.y += 180.0f;
	mySettings.myEndRotation.z = (-mySettings.myEndRotation.z) - 360.0f;
	mySettings.myEndRotation *= (PI / 180.0f);
	myStart = Quaternion::CreateFromYawPitchRoll(mySettings.myStartRotation.y, mySettings.myStartRotation.x, mySettings.myStartRotation.z);
	myEnd = Quaternion::CreateFromYawPitchRoll(mySettings.myEndRotation.y, mySettings.myEndRotation.x, mySettings.myEndRotation.z);

	Enabled(false);
}

void CRotateResponse::Update()
{
	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		if (mySettings.myDelay >= myTime)
			return;

		ToggleHasBeenDelayed();
		myTime -= mySettings.myDelay;
	}

	GameObject().myTransform->Rotation(Quaternion::Slerp(myStart, myEnd, myTime / mySettings.myDuration));

	if (myTime >= mySettings.myDuration)
		Enabled(false);
}

void CRotateResponse::OnRespond()
{
	Enabled(true);
}
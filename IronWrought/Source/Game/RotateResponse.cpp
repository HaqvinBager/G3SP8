#include "stdafx.h"
#include "RotateResponse.h"
#include "TransformComponent.h"
#define PI 3.141592f

CRotateResponse::CRotateResponse(CGameObject& aParent, const SSettings<Quaternion>& someSettings)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
	/*
	
	mySettings.myStart.x = (-mySettings.myStart.x) - 360.0f;
	mySettings.myStart.y += 180.0f;
	mySettings.myStart.z = (-mySettings.myStart.z) - 360.0f;
	mySettings.myStart *= (PI / 180.0f);

	mySettings.myEnd.x = (-mySettings.myEnd.x) - 360.0f;
	mySettings.myEnd.y += 180.0f;
	mySettings.myEnd.z = (-mySettings.myEnd.z) - 360.0f;
	mySettings.myEnd *= (PI / 180.0f);
	myStart = Quaternion::CreateFromYawPitchRoll(mySettings.myStart.y, mySettings.myStart.x, mySettings.myStart.z);
	myEnd =	  Quaternion::CreateFromYawPitchRoll(mySettings.myEnd.y, mySettings.myEnd.x, mySettings.myEnd.z);
	
	*/

	HasBeenActivated(false);
}

void CRotateResponse::Update()
{
	if (!HasBeenActivated())
		return;

	myTime += CTimer::Dt();

	if (!HasBeenDelayed())
	{
		if (mySettings.myDelay >= myTime)
			return;

		ToggleHasBeenDelayed();
		myTime -= mySettings.myDelay;
	}

	float t = std::clamp((myTime / mySettings.myDuration), 0.0f, 1.0f);
	GameObject().myTransform->Rotation(Quaternion::Slerp(mySettings.myStart, mySettings.myEnd, t));

	if (myTime >= mySettings.myDuration)
		HasBeenActivated(false);
}

void CRotateResponse::OnRespond()
{
	HasBeenActivated(true);
}
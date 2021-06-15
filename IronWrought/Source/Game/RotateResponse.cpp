#include "stdafx.h"
#include "RotateResponse.h"
#include "TransformComponent.h"
#define PI 3.141592f

CRotateResponse::CRotateResponse(CGameObject& aParent, const SSettings<Quaternion>& someSettings)
	: IResponseBehavior(aParent)
	, myTime(0.0f)
	, mySettings(someSettings)
{
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